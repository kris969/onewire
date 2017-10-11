// 
// 
// AXP209-internal-temperature USB-voltage USB-current battery-voltage [+-]battery-current
// https://bbs.nextthing.co/t/pwrstat-c-talk-to-axp209-over-i2c-in-c/6710?u=yoursunny

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>


#include "AXP209.h"

int fd ;

AXP209::AXP209(const char* i2c_bus, unsigned char address)
{
	
	fd = open(i2c_bus, O_RDWR);
	if (fd < 0) 
	{
		//perror("i2c_init|cannot open I2C bus");
		//return 0 ;
	}
	
	int ret = ioctl(fd, I2C_SLAVE_FORCE, address);
	if (ret < 0) 
	{
		//printf("i2c_init|cannot talk to AXP209\n");
		//return 0 ;
	}
	
	int adcEnable1 = readReg(fd, 0x82);
	int adcEnable2 = readReg(fd, 0x83);
	
	bool needWait = 0;
	//0xcc : voltage + current ADC Enable
	if ((adcEnable1 & 0xcc) != 0xcc) 
	{
		adcEnable1 |= 0xcc;
		writeReg(fd, 0x82, adcEnable1);
		needWait = 1;
	}
	
	//0x80 : AXP209 Internal temperature monitoring ADC Enable
	if ((adcEnable2 & 0x80) != 0x80) 
	{
		adcEnable2 |= 0x80;
		writeReg(fd, 0x83, adcEnable2);
		needWait = 1;
	}
	
	if (needWait) sleep(1);

}

AXP209::~AXP209()
{
}



void AXP209::writeReg(int fd, int reg, uint8_t val)
{
	uint8_t cmdbuf[2];
	cmdbuf[0] = reg  & 0xFF;
	cmdbuf[1] = val;
	
	int res = write(fd, cmdbuf, sizeof(cmdbuf));
	if (res < 0) 
	{
	  //printf("writeReg|cannot write register\n");
	  //exit(1);
	}
}


int AXP209::readReg(int fd, int reg)
{
	uint8_t regbuf[1];
	regbuf[0] = reg  & 0xFF;
	int res = write(fd, regbuf, sizeof(regbuf));
	if (res < 0) 
	{
	  //printf("readReg|cannot write register\n");
	  //exit(1);
	}
	
	
	uint8_t valbuf[1];
	res = read(fd, valbuf, sizeof(valbuf));
	if (res < 0) 
	{
	  //printf("readReg|cannot read register\n");
	  //exit(1);
	}

	return valbuf[0];
}



int AXP209::readAdc(int fd, int regH, int regL, int nblowBits)
{
	int h = readReg(fd, regH);
	int l = readReg(fd, regL);
	return (h << nblowBits) + l;
}




int AXP209::power_status()
{	
	
	int powerstatus = readReg(fd, 0x00);
	
	printf ("powerstatus: %0X\n", powerstatus) ;
	
	return powerstatus ;
}


int AXP209::power_mode()
{
	
	int powermode = readReg(fd, 0x01);
	printf ("powermode: %0X\n", powermode) ;

	
	return powermode ;
}


float AXP209::temperature()
{
	float temperature ;
	
	//AXP209 Internal temperature monitoring ADC Data High 8 Bit : 5E [7: 0]
	//AXP209 Internal temperature monitoring ADC Data Low 4 Bit  : 5F [3: 0]
	//Internal temperature -144.7C 0.1C 264.8C
	int temp = readAdc(fd, 0x5E, 0x5F, 4);
	
	temperature = 0.1 * temp - 144.7 ;

	return temperature ;
}

float AXP209::VBUS_voltage()
{
	float voltage = 0 ;
	//BIT5 : VBUS presence indication
	//0: VBUS Does not exist;1: VBUS Exist
	int powerstatus = readReg(fd, 0x00);
	bool hasVbus = (powerstatus & 0x20) != 0;
	if (hasVbus) 
	{
		// VBUS Voltage ADC Data High 8 Bit : 5A [7: 0]
		// VBUS Voltage ADC Data Low 4 Bit : 5B [3: 0]
		// VBUS voltage 0mV 1.7mV 6.9615V
		int vbusVolt = readAdc(fd, 0x5A, 0x5B, 4);
		voltage = 1.7 * vbusVolt / 1000 ;
	}

	return voltage ;
}

float AXP209::VBUS_current()
{
	float current = 0 ;
	//BIT5 : VBUS presence indication
	//0: VBUS Does not exist;1: VBUS Exist
	int powerstatus = readReg(fd, 0x00);
	bool hasVbus = (powerstatus & 0x20) != 0;
	if (hasVbus) 
	{
		// VBUS Current ADC Data High 8 Bit : 5C [7: 0]
		// VBUS Current ADC Data Low 4 Bit : 5D [3: 0]
		// VBUS current 0mA 0.375mA 1.5356A
		int vbusCurrent = readAdc(fd, 0x5C, 0x5D, 4);
		current = 0.375 * vbusCurrent ;
	}
	return current ;
}

bool AXP209::presence_tension(void) 
{
	bool retcode = true ;
	
	if (VBUS_voltage() == 0) retcode = false ;
	
	return retcode ;
}

bool AXP209::battery_connected()
{
	bool retcode = false ;
	int powermode ;
	pst_power_mode p_power_mode = (pst_power_mode) &powermode ;
	
	
	powermode = readReg(fd, 0x01);

	if (p_power_mode->battery_connected) retcode = true ;
	
	return 	retcode ;
}

bool AXP209::activation_mode()
{
	bool retcode = false ;
	int powermode ;
	pst_power_mode p_power_mode = (pst_power_mode) &powermode ;
	
	powermode = readReg(fd, 0x01);

	if (p_power_mode->activation_mode) retcode = true ;
	
	return 	retcode ;
}




float AXP209::battery_voltage()
{
	float voltage = -1 ;
	
	//if (battery_is_connected()) 
	{ // battery is connected
		// Battery voltage is high 8 Bit : 78 [7: 0]
		// Battery voltage is low 4 Bit : 79 [3: 0]
		// Battery Voltage 0mV 1.1mV 4.5045V
		int battVolt = readAdc(fd, 0x78, 0x79, 4);
		voltage = 1.1 * battVolt / 1000 ;
		
	}
	return voltage ;
}


bool AXP209::battery_charging()
{
	int powermode ;
	pst_power_mode p_power_mode = (pst_power_mode) &powermode ;
	
	powermode = readReg(fd, 0x01);
	
	 //printf("-->over_temperature: %d\n", p_power_mode->over_temperature) ;
	 //printf("-->is_charging: %d\n", 		p_power_mode->is_charging) ;
	 //printf("-->battery_connected: %d\n",p_power_mode->battery_connected) ;
	 //printf("-->charger_activation_mode: %d\n",p_power_mode->charger_activation_mode) ;
	 //printf("-->charging_current_nok: %d\n", 	p_power_mode->charging_current_nok) ;
	 
	return (p_power_mode->is_charging) ; 
}
	


float AXP209::battery_charging_current(void)
{
	float current ;
	
	// 7A [7: 0] Battery charge current high 8 Bit
	// 7B [3: 0] Battery charge current low 4 Bit
	// Bat discharge current 0mA 0.5mA 4.095A
	
	int battCurrent = readAdc(fd, 0x7A, 0x7B, 4);
	current = 0.5 * battCurrent ;
	
	
	
	return current ;
}



float AXP209::battery_discharging_current(void) 
{
	float current ;
	
	// 7C [7: 0] Battery discharge current high 8 Bit
	// 7D [4: 0] Battery discharge current low 5 Bit
	// Bat charge current 0mA 0.5mA 4.095A
	
	int battCurrent = readAdc(fd, 0x7C, 0x7D, 5);
	current = 0.5 * battCurrent ;
	
	
	return current ;
}








