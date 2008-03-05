#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
//#include <strings.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include "logger.h"


module mod_list[LOG_MAXLOGS];
int mod_cnt;

int log_lvl;

char log_inbuf[LOG_MAXTEXT];
char log_tbuf[LOG_MAXTEXT];
int logger_handle;

//LARGE_INTEGER logger_pf; //performance counter frequency.  don't need this, don't care

/////////////////////////////////////////////////////////////////////////////
//prototypes
int logger_verif_valid(int handle);
int logger_verif_active(int desired, int handle);
int log_hvalid(int handle);
void logger_clear();
void logger_delete();
int  logger_findslot();

//init
int logger_init(char* t)
{
	char logname[64];
//printf("entering init\n");
	log_lvl = LOGSEV_DFLT;
	logger_clear();
	logger_handle = -1;
	if (t)
	{
		sprintf(logname, "logger_%s",t);
		logger_handle = logger_regmodule(logname);
	}
	else
		logger_handle = logger_regmodule("logger");
	if (logger_handle != 0)
	{
		printf("LOGGER FAILURE, couldn't open own logfile, failed reg\n");
		return -1;
	}
	if (logger_openhandle(logger_handle) == 0)
	{
		printf("LOGGER FAILURE, couldn't open own logfile, failed open\n");
		return -1;
	}
	log(logger_handle, LOGSEV_STATUS, "successfully started logger");
	//QueryPerformanceFrequency((LARGE_INTEGER*)&logger_pf);
	//snprintf(log_tbuf, LOG_MAXTEXT, "perf counter freq is %d:%d", logger_pf.HighPart, logger_pf.LowPart);
	//log(logger_handle, LS_D, log_tbuf);
	
//printf("leaving init\n");
	return 0;
}

void logger_exit()
{
	log(logger_handle, LOGSEV_STATUS, "shutting down logger");
	logger_closehandle(logger_handle);
	logger_unreg(logger_handle);
	logger_delete();
	logger_clear();
}

/////////////////////////////////////////////////////////////////////////////
//publics

int lnprintf(int h, int sev, char* fmt, ...)
{
	if (!log_hvalid(h))
		return 0;
	if (mod_list[h].log_lvl < sev)
		return 0;
	if (!fmt)
		return 0;
	va_list lst;
	va_start(lst, fmt);
	vsnprintf(log_inbuf, LOG_MAXTEXT, fmt, lst);
	va_end(lst);
	log (h,sev,log_inbuf);
	return 1;
}

void log(int handle, int sev, char* text)
{
//um, this really should be first, as it's fast, and a critical kick-out
	if (log_hvalid(handle))
	{
		if (sev > mod_list[handle].log_lvl)
			return;
	}
	else
	{
		if (sev > log_lvl)
			return;
	}

	char tbuf[LOG_MAXTEXT+1];
	char obuf[LOG_MAXOUT];
	char timebuf[64];

	struct timeb cursecs;
	struct tm* curtime;
	ftime(&cursecs);
	curtime = localtime(&(cursecs.time));
	LARGE_INTEGER pc;

	if ((1) || (log_lvl > LS_D))
	{
		QueryPerformanceCounter(&pc);
		snprintf(timebuf, 64, ":%02d%02d:%02d%02d:%02d.%03d.%08x:%d: ",
					curtime->tm_mon+1, curtime->tm_mday,  //stupid month is 0-based
					curtime->tm_hour,  curtime->tm_min,
					curtime->tm_sec,   cursecs.millitm, pc.LowPart, sev);
	}
	else
		snprintf(timebuf, 64, ":%02d%02d:%02d%02d:%02d.%02d:%d: ",
					curtime->tm_mon+1, curtime->tm_mday,  //stupid month is 0-based
					curtime->tm_hour,  curtime->tm_min,
					curtime->tm_sec,   cursecs.millitm/10, sev);
	


	memset(tbuf,0,LOG_MAXTEXT+1);
	memset(obuf,0,LOG_MAXOUT);
	if (!text)
		strcpy(tbuf,"(empty msg)");
	else
		strncpy(tbuf, text, LOG_MAXTEXT);

	if (!log_hvalid(handle))
	{
		if (handle == logger_handle)  //shoudln't ever see this, but it will prevent an infinite loop
		{
			printf("%sLOGGER FAILURE, couldn't log() to own logfile, handle oob\n",timebuf);
			printf("                text was [%s]\n",text);
			return;
		}
		lnprintf(logger_handle, LS_F, "%s text:[%s] passed in with handle [%d] out of bounds",timebuf,text,handle);
		return;
	}
	if (mod_list[handle].active != 2) //bad handle
	{
		if (handle == logger_handle)  //shoudln't ever see this, but it will prevent an infinite loop
		{
			printf("%sLOGGER FAILURE, couldn't log() to own logfile, inactive (%d) handle\n", timebuf,mod_list[handle].active);
			printf("                text was [%s]\n",text);
			return;
		}
		lnprintf(logger_handle, LS_F, "%s text:[%s] passed in with inactive (%d) handle: %d\n",timebuf,tbuf, mod_list[handle].active, handle);
		return;
	}

	//main format string.  not using the modname at the moment,
	//seems extraneous to write the same word for every entry in the file
	snprintf(obuf, LOG_MAXOUT, "%s%s\n", timebuf,tbuf);
	fwrite(obuf, 1, strlen(obuf), mod_list[handle].logfile);
}

//registers module
//0 negative are failure returns, 0 should always be claimed by logger
//thus, 0 is a valid return for the logger (only)
int logger_regmodule(char* modname)
{
	int slot;
//printf("entering reg\n");
	if (!modname)
	{
		lnprintf(logger_handle, LS_F, "no module name passed in");
		return 0;
	}
	if (!strlen(modname))
	{
		lnprintf(logger_handle, LS_F, "module name passed in [%s] is too short (0)",modname);
		return 0;
	}
	if (strlen(modname) > LOG_MAXMODNAMELEN)
	{
		lnprintf(logger_handle, LS_F, "module name passed in [%s] is too long",modname);
		return 0;
	}
//printf("starting reg\n");
		
	slot = logger_findslot();
//printf("reg slot = %d\n",slot);
	if (slot == -1)
	{
		lnprintf(logger_handle, LS_F, "module name passed in [%s] is too long",modname);
		return 0;
	}
	mod_list[slot].modname = strdup(modname);
	mod_cnt++;
	mod_list[slot].active = 1;
	mod_list[slot].log_lvl = log_lvl;  //local log level set to global
	if (slot != 0)  //do'nt bother with up/down for the main logger
	{
		lnprintf(logger_handle, LS_ST, "registered handle %d for %s", slot, modname);
	}
//printf("leaving reg\n");
	return slot;
}

//starts actual file
//0 return is failure
int logger_openhandle(int handle)
{
	char logname[LOG_MAXMODNAMELEN+5];
//printf("entering open, handle = %d\n",handle);
	if (!logger_verif_valid(handle))	return 0;
	if (!logger_verif_active(1,handle)) return 0;

//printf("starting open\n");
	sprintf(logname,"log_%s.txt",mod_list[handle].modname);

	mod_list[handle].logfile = fopen(logname, "w");
	if (!(mod_list[handle].logfile)) //failed to open
	{
		lnprintf(logger_handle, LS_F, "failed to open logfile %s for handle %d, err =%d(%s)",logname,handle,errno,strerror(errno));
		return 0;
	}
	//set logfile buffering based on current dbg level
	mod_list[handle].active = 2;
	if (mod_list[handle].log_lvl > LOGSEV_DFLT)
		setvbuf(mod_list[handle].logfile, NULL, _IONBF, 0);
	if (handle != 0)  //don't bother with up/down for the main logger
	{
		snprintf(log_tbuf, LOG_MAXTEXT, "opened logfile %s for handle %d at level %d.",logname, handle, mod_list[handle].log_lvl);
		if (mod_list[handle].log_lvl > LOGSEV_DFLT)
			strcat(log_tbuf, "  buffering disabled");
		log(logger_handle, LS_ST, log_tbuf);
	}
//printf("leaving open\n");
	return 1; //success
}

//flushes, closes file
int logger_closehandle(int handle)
{
//printf("entering close\n");
	if (!logger_verif_valid(handle))	return 0;
	if (!logger_verif_active(2,handle)) return 0;

//printf("starting close\n");
//
	if (handle != 0)  //don't bother with up/down for the main logger
	{
		lnprintf(logger_handle, LS_ST, "closed logfile handle %d for %s", handle, mod_list[handle].modname);
	}

	fclose(mod_list[handle].logfile);
	mod_list[handle].logfile = NULL;
	mod_list[handle].active = 1;


//printf("leaving close\n");

	return 0;
}

//removes module entry
int logger_unreg(int handle)
{
//printf("entering unreg\n");
	if (!logger_verif_valid(handle))	return 0;
	if (!logger_verif_active(1,handle)) return 0;

//printf("starting unreg\n");
	if (handle != 0)  //do'nt bother with up/down for the main logger
	{
		lnprintf(logger_handle, LS_ST, "unregistered handle %d for %s", handle, mod_list[handle].modname);
	}

	free(mod_list[handle].modname);
	mod_list[handle].modname = NULL;
	mod_list[handle].active = 0;
	mod_cnt--;

//printf("leaving unreg\n");
	return 0;
}


//set log level
void logger_sl(int sev, int handle)
{
	if (handle == -1) //use for global
	{
		log_lvl = sev;
		lnprintf(logger_handle, LS_ST, "global log level set to %d",sev);
		if (sev > LOGSEV_DFLT)
			setvbuf(mod_list[0].logfile, NULL, _IONBF, 0);
	}
	else
	{
		//currently, this doesn't change anything.
		//it will be set per module, but not checked by log()
		if (!logger_verif_valid(handle))	return;
		mod_list[handle].log_lvl = sev;
		lnprintf(logger_handle, LS_ST, "handle %d log level set to %d",handle,sev);
	}
}

/////////////////////////////////////////////////////////////////////////////
//privates

//assumes data are invalid, wipes entire list
void logger_clear()
{
	int i;
//printf("entering clear\n");
	mod_cnt = 0;
	for (i=0; i<LOG_MAXLOGS; i++)
	{
		mod_list[i].modname = NULL;
		mod_list[i].logfile = NULL;
		mod_list[i].active = 0;
	}
//printf("leaving clear\n");
}

//assumes data are valid, frees/closes active entries
void logger_delete()
{
	int i;
	for (i=0; i<LOG_MAXLOGS; i++)
	{
		if (mod_list[i].modname)
			free(mod_list[i].modname);
		mod_list[i].modname = NULL;
		if (mod_list[i].logfile)
			fclose(mod_list[i].logfile);
		mod_list[i].logfile = NULL;
		mod_list[i].active = 0;
	}
	mod_cnt = 0;
}

//find free slot in our list
//returns -1 for failure
int logger_findslot()
{
	int i;
//printf("entering findslot\n");
	if (mod_cnt >= LOG_MAXLOGS)  //should never be greater, but still
	{
		log(logger_handle, LOGSEV_FAIL, "mod_cnt says we are full...");
		return -1;
	}
//printf(" findslot: not past max count\n");
	if (mod_cnt == 0)
	{
//printf("leaving findslot\n");
		return 0;
	}
//printf(" findslot: not at count 0\n");
	for (i=0; i<LOG_MAXLOGS; i++)
		//check for unused handle.  actual logfile may not be opened yet
		if (!(mod_list[i].active))  //slot is empty
			return i;
//printf(" findslot: didn't find an inactive slot\n");
	log(logger_handle, LOGSEV_FAIL, "failed to find a useful slot, but mod_cnt says we aren't full...");
//printf("leaving findslot\n");
	return -1; //shouldn't get here...
}

int log_hvalid(int handle)
{
	if (handle < 0)
		return 0;
	if (handle >= LOG_MAXLOGS)
		return 0;
	return 1;
}

//0 is failure, nonzero is success
int logger_verif_valid(int handle)
{
	if (!log_hvalid(handle))
	{
		lnprintf(logger_handle, LS_F, "handle passed in [%d] is out of bounds",handle);
		return 0;
	}
	return 1;
}

//0 is failure, nonzero is success
int logger_verif_active(int desired, int handle)
{
	if (mod_list[handle].active != desired) //still have an open file here
	{
		lnprintf(logger_handle, LS_F, "handle %d should be active(%d) but is active(%d)",handle,desired, mod_list[handle].active);
		return 0;
	}
	return 1;
}
