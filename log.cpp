
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <time.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"

#define MAX_PATH 50


log::log(const char* path, const char* ext)
{
		set_path(path) ;
		set_ext(ext) ;
}

log::~log()
{
}

void log::set_path(const char* path)
{
	strncpy(_path, path, sizeof(_path) ) ; 
}


void log::set_ext(const char* ext)
{
	strncpy(_ext, ext, sizeof(_ext) ) ; 
}




void log::write(const char* name, char* log_text, bool screen)
{

    FILE *fp ;
    char filename[MAX_PATH+1] ;
    char trace[MAX_PATH+1] ;

   
	// set timestamp
    time_t timestamp;
	struct tm *t;
	
	timestamp = time (NULL); 
	t = localtime(&timestamp);
    sprintf ( _horodatage, "%04d/%02d/%02d %02d:%02d", 1900+t->tm_year, 1+t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min) ;
     
    strncpy(filename, _path, MAX_PATH) ;
	strncat(filename, name, MAX_PATH - strlen(filename)) ;
	strncat(filename, ".", MAX_PATH - strlen(filename)) ;
	strncat(filename, _ext, MAX_PATH - strlen(filename)) ;
     
    strncpy(trace, "/tmp/", MAX_PATH) ;
	strncat(trace, "trace", MAX_PATH - strlen(filename)) ;
	strncat(trace, ".", MAX_PATH - strlen(filename)) ;
	strncat(trace, _ext, MAX_PATH - strlen(filename)) ;

    fp = fopen (filename, "a") ;
    if (fp != NULL)
    { // file is open
        fprintf(fp, "%s %s\n", _horodatage, log_text);
        fclose(fp);

    } // file is open
    
    
    if (!screen) return ;
    return ;
    
    fp = fopen (trace, "a") ;
    if (fp != NULL)
    { // file is open
		char screen_log[80] ;
		
		strcpy(screen_log, _horodatage) ;
		strncat(screen_log, " ",sizeof(screen_log) - strlen(screen_log)) ;
		strncat(screen_log, log_text, sizeof(screen_log) - strlen(screen_log));
		int nb_space = 40 - strlen(log_text) ;
		for (int i = nb_space; i > 1; i--) strncat(screen_log, " ",sizeof(screen_log) - strlen(screen_log)) ;
		strncat(screen_log, "| ",sizeof(screen_log) - strlen(screen_log)) ;
		strncat(screen_log, name, sizeof(screen_log) - strlen(screen_log)) ;
		
		fprintf(fp, "%s\n", screen_log) ;
        fclose(fp);

	} // file is open
    
    return ;
}
