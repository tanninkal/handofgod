#include "logger.h"

int main_lh; //logger handle
char main_tbuf[LOGGER_MAXTEXT]; //logger buff

int die(int ret);

int main(int agrc, char** argv)
{
	logger_init("main");
	logger_sl(LS_D);
	
	main_lh = logger_regmodule("main");
	logger_sl(main_lh, LS_D);
	logger_openhandle(main_lh);
	
	log(main_lh, LOGSEV_STATUS, "main up!");
	
	return die(0);
}

int die(int ret)
{
	log(main_lh, LS_ST, "beginning main shutdown");
	lnprintf(main_lh, LS_ST, "exit code: %d", ret);
	logger_closehandle(main_lh);
	logger_unreg(main_lh);
	log(main_lh, LS_ST, "main shutdown complete");
	logger_exit();
	return ret;
}