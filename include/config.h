#ifndef __CONFIG_H_
#define __CONFIG_H_

int read_config(const char *file);
const char * conf_strerror(unsigned int error);

// define state error code
#define E_CONF_NO_ERROR 0
#define E_CONF_FILE_END 0
#define E_CONF_INVALID_STATE   1
#define E_CONF_ILLEGAL_CHAR 2 // illegal char
#define E_CONF_FILE_NOT_EXIST 3 // file don't exist
#define E_CONF_FILE_READ 4  // fail to read config file 
#define E_CONF_TOKEN_ILLEGAL  5 // illegal token
#define E_CONF_INVALID_END 6 // invalid end
#endif
