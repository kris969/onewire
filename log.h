// log.h
#ifndef LOG_H
#define LOG_H


#define MAX_PATH 50

 
class log
{
    private:
		char _horodatage[20] ;
        char _path[MAX_PATH] ;
        char _ext[3] ;
              
        

    public:
		log(const char* path, const char* ext) ;
		~log() ;
		void set_path(const char* path) ;
		void set_ext(const char* ext) ;
		void write(const char* name, char *log_text, bool screen) ;



};




#endif
