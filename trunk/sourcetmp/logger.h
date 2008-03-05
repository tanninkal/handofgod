#include <stdio.h>

#ifndef _LOGGER_H_
#define _LOGGER_H_

#define LOG_MAXLOGS	32			//max number of log handles
#define LOG_MAXMODNAMELEN	32	//max len of module name
#define LOG_MAXTEXT	256	//maximum number of chars in a log msg passed in
#define LOG_MAXOUT	512	//max size of an assembled string


#define LOGSEV_TRACE		10		//"HERE" type calls
#define LOGSEV_INOUT		8		//entering/leaving function
#define LOGSEV_VALUE		6		//debug, value=%d
#define LOGSEV_DFLT		5		//default, generic
#define LOGSEV_STATUS	4		//systems up/down
#define LOGSEV_WARN		3		//warning, using something inactive or the like
#define LOGSEV_FAIL		2		//failed to do something
#define LOGSEV_CRIT		0		//critical failure

#define LS_TR	LOGSEV_TRACE
#define LS_IO	LOGSEV_INOUT
#define LS_VAL	LOGSEV_VALUE
#define LS_D	LOGSEV_DFLT
#define LS_ST	LOGSEV_STATUS
#define LS_W	LOGSEV_WARN
#define LS_F	LOGSEV_FAIL
#define LS_CR	LOGSEV_CRIT

#define snprintf _snprintf

typedef struct
{
	char* modname;
	FILE* logfile;
	int active;	//0 for nothing, 1 for modname in, 2 for logfile ready
	int log_lvl;
}	module;

int	logger_init(char* t);
void	logger_exit();

void log(int handle, int sev, char* text);
int lnprintf(int h, int sev, char* fmt, ...);

void logger_sl(int sev, int handle = -1);
int logger_regmodule(char* modname);
int logger_openhandle(int handle);
int logger_closehandle(int handle);
int logger_unreg(int handle);



#endif
