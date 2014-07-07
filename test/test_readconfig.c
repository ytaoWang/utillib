#include <stdio.h>
#include <stdlib.h>

#include "config.h"

int main(int argc, char *argv[]) 
{
	int ret;

	if( argc < 2 ) {
		fprintf(stderr,"usage:%s file.\n",argv[0]);
		return -1;
	}

	/*
	 * check file exist or not
	 * analyze config file
	 */
	ret = read_config(argv[1]);
	if(ret != E_CONF_NO_ERROR) {
		fprintf(stderr,"config file err:%s\n",conf_strerror(ret));
	}
	return 0;
}
