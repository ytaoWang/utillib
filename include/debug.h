#ifndef __DEBUG_H_
#define __DEBUG_H_

#ifdef __cplusplus
extern "C"  {
#include <cstdio>
#else
#include <stdio.h>
#endif

#define fdebug_error(f,a...)					\
	do {																\
		fprintf(stderr,"[ERROR]:file:%s,line:%d,function:%s [MSG]:",__FILE__,__LINE__,__func__); \
		fprintf(stderr,f,##a);											\
		fprintf(stderr,"\n");											\
	} while(0)

#define fdebug_warn(f,a...)					\
	do {																\
		fprintf(stderr,"[WARN]:file:%s,line:%d,function:%s [MSG]:",__FILE__,__LINE__,__func__); \
		fprintf(stderr,f,##a);											\
		fprintf(stderr,"\n");											\
	} while(0)

#ifdef __FDEBUG_DEBUG_

#define fdebug_debug(f,a...)					\
	do {																\
		fprintf(stderr,"[DEBUG]:file:%s,line:%d,function:%s [MSG]:",__FILE__,__LINE__,__func__); \
		fprintf(stderr,f,##a);											\
		fprintf(stderr,"\n");											\
	} while(0)

#define fdebug_test(f,a...)					\
	do {																\
		fprintf(stderr,"[TEST]:file:%s,line:%d,function:%s [MSG]:",__FILE__,__LINE__,__func__); \
		fprintf(stderr,f,##a);											\
		fprintf(stderr,"\n");											\
	} while(0)

#else

#define fdebug_debug(f,a...)
#define fdebug_test(f,a...)
#endif

#ifdef __cplusplus
}
#endif

#endif
