// AX209.h
#ifndef AX209_H
#define AX209_H

// POWER CONTROL REGISTER CODES 
#define REG_POWER_STATUS	0x00 	// Power Status Register R
#define REG_POWER_MODE		0x01 	// Power Mode/Charging Status Register R
//#define REG_	0x02 	// OTG VBUS Status Register R
//#define REG_	0x04-0F	// Data buffer register R / W 00H
//#define REG_	0x12 	// DC-DC2 / 3 & LDO2 / 3/4 & EXTEN Control Register R / W XXH
//#define REG_	0x23 	// DC-DC2 Voltage setting register R / W XXH
//#define REG_	0x25 	// DC-DC2/LDO3 Voltage ramp parameter R / W 00H
//#define REG_	0x27 	// DC-DC3 Voltage setting register R / W XXH
//#define REG_	0x28 	// LDO2 / 3 Voltage setting register R / W XXH
//#define REG_	0x30 	// VBUS-IPSOUT Path setting register R / W 60H
//#define REG_	0x31 	// VOFF Shutdown voltage setting register R / W X3H
//#define REG_	0x32 	// Shutdown, battery detection, CHGLED Control R / W 46H
//#define REG_	0x33 	// Charge control register 1 R / W CXH
//#define REG_	0x34 	// Charge control register 2 R / W 41H
//#define REG_	0x35 	// Backup battery charging control register R / W 22H
//#define REG_	0x36 	// PEK Parameter setting register R / W 5DH
//#define REG_	0x37 	// DCDC Converter operating frequency setting R / W 08H
//#define REG_	0x38 	// Battery charging low temperature alarm setting R / W A5H
//#define REG_	0x39 	// Battery charging temperature alarm setting R / W 1FH
//#define REG_	0x3A 	// APS Low Power Level1 Setting register R / W 68H
//#define REG_	0x3B 	// APS Low Power Level2 Setting register R / W 5FH
//#define REG_	0x3C 	// Battery discharge and low temperature alarm setting register R / W FCH
//#define REG_	0x3D 	// Battery discharge temperature alarm setting R / W 16H
//#define REG_	0x80 	// DCDC Work mode setting register R / W E0H
#define REG_ADC_ENABLE_REG1	0x82 	// ADC Enable setting register 1 R / W 83H
#define REG_ADC_ENABLE_REG2	0x83	// ADC Enable setting register 2 R / W 80H
//#define REG_	0x84	// ADC Sample rate settings, TS pin Control Register R / W 32H
//#define REG_	0x85 	// GPIO [1: 0]Input range setting register R / W X0H
//#define REG_	0x86	// GPIO1 ADC IRQ Rising edge threshold setting R / W FFH
//#define REG_	0x87	// GPIO1 ADC IRQ Falling threshold setting R / W 00H
//#define REG_	0x8A	// Timer control register R / W 00H
//#define REG_	0x8B 	// VBUS Monitoring setting register R / W 00H
//#define REG_	0x8F 	// Over temperature shutdown control R / W 01H



//A 10 1010
//B 11 1011
//C 12 1100
//D 13 1101
//E 14 1110
//F 15 1111


//0xcc : voltage + current ADC Enable
//Enhanced single Cell Li-Battery and Power System Management IC
// RW - 0:Disable, 1:Enable RW 1
typedef struct {
	unsigned int ADC_battery_voltage_Enable : 1 ;	//7 Battery voltage ADC Enable
	unsigned int ADC_battery_current_Enable : 1 ;	//6 Battery current ADC Enable
	unsigned int ADC_ACIN_voltage_Enable : 1 ;		//5 ACIN Voltage ADC Enable
	unsigned int ADC_ACIN_current_Enable : 1 ;		//4 ACIN Current ADC Enable
	unsigned int ADC_VBUS_voltage_Enable : 1 ;		//3 VBUS Voltage ADC Enable
	unsigned int ADC_VBUS_current_Enable : 1 ;		//2 VBUS Current ADC Enable
	unsigned int ADC_APS_voltage_Enable : 1 ;		//1 APS Voltage ADC Enable
	unsigned int ADC_TS_voltage_Enable : 1 ;		//0 TS Pin ADC Function is enabled (Temperature Sensitive resitor to monitor battery temperature)
} st_adc1 ;

// RW - 0:Disable, 1:Enable RW 1
typedef struct {
	unsigned int ADC_AXP209_temperature_Enable : 1 ;	//7 AXP209 Internal temperature monitoring ADC Enable 0:Disable, 1:Enable RW 1
	unsigned int reserved_2 : 3 ;						//6-4 Reserved, can not be changed
	unsigned int GPIO0_temperature_Enable : 1 ;			//3 GPIO0 ADC Function is enabled 
	unsigned int GPIO1_temperature_Enable : 1 ; 		//2 GPIO1 ADC Function is enabled 
	unsigned int reserved_1 : 2 ;						//1-0 Reserved, can not be changed
} st_adc2 ;

//0x39 : 0011 1001
//0x3D : 0011 1101
typedef struct  {
	uint8_t 	boot_mode : 1 ;			// [BIT0] Indicating whether the boot source is ACIN or VBUS, 0: Boot source isn’t ACIN/VBUS； 1: Boot source is ACIN/VBUS. 
	uint8_t 	acin_vbuc_sc : 1 ;		// [BIT1] Indicating whether ACIN and VBUS input short circuit on PCB
	uint8_t 	charging : 1 ;			// [BIT2] Indicates that the battery current direction, 0:Battery discharging; 1:The battery is charging
	uint8_t 	above_vhold : 1 ;		// [BIT3] Indicating whether the VBUS voltage is above VHOLD before used.
	uint8_t 	vbus_usable : 1 ;		// [BIT4] Indicates if VBUS is useable
	uint8_t 	vbus_exist : 1 ;		// [BIT5] VBUS presence indication 0: VBUS Does not exist;1: VBUS Exist
	uint8_t 	acin_available : 1 ;	// [BIT6] Instructions ACIN Is available
	uint8_t 	acin_exist : 1 ;		// [BIT7] ACIN presence indication, 0: ACIN Does not exist;1: ACIN Exist
} st_power_status, *pst_power_status ;
	
//0x30 : 0011 0000
//0x74 : 0111 0100
typedef struct  {


	 uint8_t 	reserved_1 : 2	;			// [BIT1-0] Reserved, can not be changed
	 uint8_t 	charging_current_nok : 1 ;	// [BIT2] Indicates the charging current is less than the expected current, 0:The actual charge current equal to the desired current; 1:The actual charge current is less than the desired current
	 uint8_t 	activation_mode : 1 ;	// [BIT3] Indicate whether the battery charger entered into activation mode, 0:Did not enter the cell activation mode; 1:Has entered the cell activation mode
 	 uint8_t 	reserved_2 : 1 ;			// [BIT4] Reservations can not be changed
	 uint8_t 	battery_connected : 1 ;		// [BIT5] Battery connected indicator, 0:No battery is connected to the AXP209; 1:The battery is connected to the AXP209
	 uint8_t 	is_charging : 1 ;			// [BIT6] Charging indicator 0:Not charging or charging has been completed; 1:Charging
	 uint8_t 	over_temperature : 1 ;		// [BIT7] Indicates AXP209 over-temperature, 0:Not too hot; 1:Over Temperature

} st_power_mode, *pst_power_mode ;
	


 
class AXP209
{
    private:
        int fd ;
        
        void writeReg(int fd, int reg, uint8_t val) ;
		int readReg(int fd, int reg) ;	
        int readAdc(int fd, int regH, int regL, int nblowBits) ;
        int power_status(void) ;
        int power_mode(void) ;


        
        

    public:
    
		AXP209(const char* i2c_bus, unsigned char address) ;
		~AXP209() ;
		float temperature(void) ;
		float VBUS_voltage(void) ;
		float VBUS_current(void) ;

		bool presence_tension(void) ;
		bool battery_connected(void) ;
		bool activation_mode(void) ;

		float battery_voltage(void) ;
		bool battery_charging(void) ;
		float battery_charging_current(void) ;
		float battery_discharging_current(void) ;


		




		

};
 


#endif
