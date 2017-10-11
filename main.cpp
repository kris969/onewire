//
//
// Cablage DS18B20 douche JAUNE:DATA ORANGE:V+  MARRON/GRIS:GND
//
//
//


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <time.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


#include "AXP209.h"
#include "log.h"




#define MAX_PATH 50
#define TIME_SLOT 5 		// timeslot in minute for synchronizing temperature log writing
#define FALSE 0
#define TRUE  1


static time_t previous_timestamp = 0 ;


typedef struct
{
	char device_id[16] ;	// 28-000003430b68
	float offset ;			// offset de compensation suite à calibration
	char name[35] ;			// 
	
}	ST_W1, *PST_W1 ;

#define NB_W1_SENSOR 2

ST_W1 w1[] = {   
				{ "28-03146d2111ff", -2.5, "temperature_01"},
				{ "28-03146d25c2ff", -1.5, "temperature_02"},
				{ "28-000003430b68", 0, "temperature_douche" }
				
			} ;
			
ST_W1 w1_douche = { "28-000003430b68", 0, "temperature_douche" } ; 
				

static char w1_devices_path[] = "/sys/bus/w1/devices/" ;
//static char log_path[] = "/tmp/" ;

AXP209 chip("/dev/i2c-0", 0x34) ;
log trace("/home/chip/log/", "log") ;

// prototypes
void init(void) ;
int is_time(void) ;
int shower_time(void) ;

int log_temperature(void) ;
int log_douche(void) ;

int w1_read(char *sensor_id, float *temperature) ;
//void write_to_log(char *filename, char *log_text) ;
void log_battery(void) ;
bool presence_tension(void) ;



int main(int argc, char **argv)
{
   static bool flag_tension = false ;
   static bool flag_battery_low = false ;
   static bool flag_activation_mode = true ;
   
   trace.write("events", (char*) "Service is starting !", true) ;
 
	    
    while(1)
    { // forever
		sleep(1) ;
		
       
        
		if (chip.presence_tension())
        { // power supply ok
			if (!flag_tension)
			{
				flag_tension = true ;
				flag_battery_low = false ;

				trace.write("events", (char*) "power supply ok", true) ;

			}
        } // power supply ok
        else
        { // no power supply
			
			if (flag_tension)
			{
				flag_tension = false ;
				trace.write("events", (char*) "Warning : no power supply", true) ;

			}
			float tension ;
			// TODO faire une comparaison time pour ne pas lire en permanence
			tension = chip.battery_voltage() ;
			if ((tension < 3.9) && (!flag_battery_low))
			{
				flag_battery_low = true ;
				trace.write("events", (char*) "Battery voltage low !", true) ;
			}
 
        } // no power supply
        
        if (chip.activation_mode())
        { // activation mode activated
			if (!flag_activation_mode)
			{ // we notice the state change off --> on
				flag_activation_mode = true ;
				trace.write("events", (char*) "activation_mode change to: ON", true) ;
			} // we notice the state change off --> on
		} // activation mode activated
		else
		{ // activation mode not activated
			if (flag_activation_mode)
			{ // we notice the state change on --> off
				flag_activation_mode = false ;
				trace.write("events", (char*) "activation_mode change to: OFF", true) ;
			} // we notice the state change on --> off
		}  // activation mode not activated
        
        
		if (is_time()) 
		{ // attendre le bon moment : toutes les x minutes
			log_battery() ;
			log_temperature() ;

		} // attendre le bon moment : toutes les x minutes
        
        //log_douche() ;

    } // forever
   
    
    return 0;
}








int log_temperature()
{
    float temperature ;
    char st_temperature[20] ;
    //char filename[MAX_PATH+1] ;
    int i ;

    for(i=0; i<NB_W1_SENSOR; i++)
    {
		
		if (w1_read(w1[i].device_id, &temperature))
		{ // lecture ok du sensor
		
			// ajout de l'offset de compensation
			temperature += w1[i].offset ;
			
			sprintf( st_temperature, "%.1f" , temperature ) ;

			trace.write(w1[i].name, st_temperature, true) ;
        
		} // lecture ok du sensor
    } 
	return 0 ;
       
}


int log_douche()
{
    float temperature ;
    char st_temperature[20] ;
    //bool flag_log = false ;

	if (w1_read(w1_douche.device_id, &temperature))
	{ // lecture ok du sensor
		temperature += w1_douche.offset ;
		if (temperature > 30)
		{ // on tire de l'eau chaude
			if (shower_time())
			{
				sprintf( st_temperature, "%.1f" , temperature ) ;
				trace.write(w1_douche.name, st_temperature, false) ;
			}
		} // on tire de l'eau chaude

        
	} // lecture ok du sensor
 
	return 0 ;
       
}


void log_battery()
{
    float battery_voltage ;
    float charging_current ;
    float discharging_current ;
    float VBUS_voltage ;
	float VBUS_current ;
	float chip_current ;
    float temperature ; // temperature du AXP209

    char st_log[40] ;
    //char filename[MAX_PATH+1] ;
    
    VBUS_voltage = chip.VBUS_voltage() ;
	VBUS_current = chip.VBUS_current() ;

    battery_voltage = chip.battery_voltage() ;
    charging_current = chip.battery_charging_current() ;
    discharging_current = chip.battery_discharging_current() ;
    temperature = chip.temperature() ;

	if (VBUS_current == 0)
	{ // all current comming from battery
		chip_current = discharging_current ;
	} // all current comming from battery
	else
	{ // charger give current to chip to all system
		if (charging_current == 0)
		{ // charger is only giving current to chip 
			chip_current = VBUS_current ;
		} // charger is only giving current to chip
		else
		{ // charger is charging battery and gving current to chip
			chip_current = VBUS_current - charging_current ;
		} // charger is charging battery and gving current to chip
	} // charger give current to chip to all system

	sprintf( st_log, "%.2f %.0f %.2f" , VBUS_voltage, VBUS_current, temperature) ;
	trace.write("vbus", st_log, true) ;
    

	sprintf( st_log, "%.2f %.0f %.0f" , battery_voltage, charging_current, discharging_current ) ;
	trace.write("batterie", st_log, true) ;
	
	sprintf ( st_log, "%.0f", chip_current) ;
	trace.write("chip", st_log, true) ;
	
}


//
// return :
// TRUE si lecture ok
// FALSE en cas d'erreur de lecture
// valeur de la temperature
int w1_read(char *sensor_id, float *temperature)
{
	int retcode = TRUE ;
	int fd ;
    char devPath[40] ;
    char buf[256];     // Data from device
    char tmpData[6];   // Temp C * 1000 reported by device 
	ssize_t numRead;



	sprintf(devPath, "%s%s/w1_slave", w1_devices_path, sensor_id);
	fd = open(devPath, O_RDONLY);
	if(fd != -1)
	{ // file open : ok
		
		while((numRead = read(fd, buf, 256)) > 0)
		{
			memset(tmpData, '\0', sizeof(tmpData)) ; 
			strncpy(tmpData, strstr(buf, "t=") + 2, 5);
			//printf("tmpData:%s\n", tmpData) ; 
			*temperature = strtof(tmpData, NULL) / 1000 ;
		}
		close(fd);
		
	} // file open : ok
	else
	{
		retcode = FALSE ;
		//perror ("Couldn't open the w1 device.");
	}

    return retcode ;   
}






//
// return TRUE if elapse time since last trig is more than TIME_SLOT
// return FALSE if time slot isn't reached
//
int is_time()
{
	// TIME_SLOT
	// static time_t previous_timestamp ;
	int retcode = FALSE ;
	
	time_t current_timestamp;
	struct tm *t;


	current_timestamp = time (NULL); 
	t = localtime(&current_timestamp);


	if (t->tm_min % TIME_SLOT == 0)
	{ // we are on a slot, regarding to TIME_SLOT
		if ( !previous_timestamp ||	(current_timestamp - previous_timestamp) >= TIME_SLOT*60) 
		{
			previous_timestamp = current_timestamp ;
			retcode = TRUE ;
		}
	} // we are on a slot, regarding to TIME_SLOT

   
	return retcode ;
}



int shower_time()
{
	// TIME_SLOT
	// static time_t previous_timestamp ;
	static time_t previous_timestamp = 0 ;
	int retcode = FALSE ;
	
	time_t current_timestamp;
	//struct tm *t;


	current_timestamp = time (NULL); 
	//t = localtime(&current_timestamp);


	if ( !previous_timestamp ||	(current_timestamp - previous_timestamp) >= 30) 
	{
		previous_timestamp = current_timestamp ;
		retcode = TRUE ;
	}

   
	return retcode ;
}


