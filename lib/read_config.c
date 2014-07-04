/**
 * parse config file
 * config style:
   
   block1 {
   	name = jack # this is comment
	sex = male # female
	age = 18  # age
   }
   
   block2 {
   	name = kitty
	sex = female
	age = 36
   }
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define _CONFIG_BUFFER 1024
#define _CONFIG_TOKEN 512

// define special character, comment character,newline,space character,split character,
#define _COMMENT_CHAR   '#'
#define _NEWLINE_CHAR   '\n'
#define _SPACE_CHAR     ' '
#define _TAB_CHAR       '\t'
#define _SPLIT_CHAR     '='
#define _SYMBOL_LEFT_CHAR '{'
#define _SYMBOL_RIGHT_CHAR '}'
#define _UNDERLINE_CHAR  '_'

// open debug mode
#ifndef __FDEBUG_DEBUG_
#define __FDEBUG_DEBUG_
#endif
#include "debug.h"

// define state operation
#define STATE_FUNCTION_NAME(name) (on##name)

// define state error code
#define E_CONF_NO_ERROR 0
#define E_CONF_FILE_END 0
#define E_CONF_INVALID_STATE   1
#define E_CONF_ILLEGAL_CHAR 2 // illegal char
#define E_CONF_FILE_NOT_EXIST 3 // file don't exist
#define E_CONF_FILE_READ 4  // fail to read config file 

#define E_CONF_MIN_ERRNO  0
#define E_CONF_MAX_ERRNO 4

#define N_(s) (s)
#define _(s)  ((const char *)s)

const char * const conf_errlist[E_CONF_MAX_ERRNO + 1] = {
	N_("No error"),  /* E_CONF_NO_ERROR 0 */
	N_("Config invalid state"), /* E_CONF_INVALID_STATE */
	N_("Illegal character"), /* E_CONF_ILLEGAL_CHAR */
	N_("File don't exist"), /* E_CONF_FILE_NOT_EXIST */
	N_("Fail to read config file"), /* E_CONF_FILE_READ */
};

enum STATE {
	STATE_START = 0,
    MODULE_START,
	MODULE_END,
	BLOCK_START,
	BLOCK_END,
	SYMBOL_KEY,
	SYMBOL_VALUE,
	SYMBOL_COMMENT,
	STATE_END
};


typedef struct {
    size_t line;
    size_t col;
	size_t pos; // conf buffer's position
	size_t len; // conf buffer's length
	unsigned int symbol; // delimiter
    enum STATE state; // now state
	char *buffer; // conf's buffer
    char *msg;  
	char *token; // current token
	FILE *file; // config file fd
} context_config_t;

typedef int (*config_func_t)(context_config_t *context);

typedef struct {
	enum STATE state;
	config_func_t func;
	const char * str;
} context_state_operation;

/*
 * define error number for debugging
 */

static unsigned int errorno_config;

static int _read_config(const char *filename);
static int get_token(context_config_t *context);
static int _parse_state(context_config_t *context);

// state operation
static int on_module_start(context_config_t *context);
static int on_module_end(context_config_t *context);
static int on_block_start(context_config_t *context);
static int on_block_end(context_config_t *context);
static int on_symbol_key(context_config_t *context);
static int on_symbol_value(context_config_t *context);
static int on_symbol_comment(context_config_t *context);

static int __skip_buffer(const char *buffer,size_t *pos, unsigned int c);


static context_state_operation state_func[] = {
	{ STATE_START,NULL,"function:NULL"},
	{ MODULE_START, on_module_start,"on_module_start" },
	{ MODULE_END, on_module_end,"on_module_end" },
	{ BLOCK_START, on_block_start,"on_block_start" },
	{ BLOCK_END, on_block_end,"on_block_end" },
	{ SYMBOL_KEY, on_symbol_key,"on_symbol_key" },
	{ SYMBOL_VALUE, on_symbol_value,"on_symbol_value" },
	{ SYMBOL_COMMENT, on_symbol_comment,"on_symbol_comment" },
	{ STATE_END, NULL,"function:NULL" }
};


const char * conf_strerror(unsigned int error)
{
	if((error < E_CONF_MIN_ERRNO) || (error > E_CONF_MAX_ERRNO) )
		return _("Unknown error");
	else
		return (conf_errlist[error]);
}

static int inline __isnewline(int c)
{
	return c == _NEWLINE_CHAR;
}
static int inline __issplit(int c)
{
	return c == _SPLIT_CHAR;
}
static int inline __iscomment(int c) 
{
	return c == _COMMENT_CHAR;
}
static int inline __isspace(int c)
{
	return c == _SPACE_CHAR || c == _TAB_CHAR;
}

static int inline __isunderline(int c)
{
	return c == _UNDERLINE_CHAR;
}

static int inline __issymbol(int c)
{
	return c == _SYMBOL_LEFT_CHAR || c == _SYMBOL_RIGHT_CHAR;
}

static int __istoken(const char *token)
{
	if(!(__isunderline(*token) || isalpha(*token)))
		return 0;

	while(*token != '\0' && isalnum(*token++))
		;

	return *(token-1) == '\0';

}

static void inline __reset(char *token, unsigned int *symbol)
{
	memset(token,'\0',_CONFIG_TOKEN);
	*symbol = 0;
}

static void set_context_state(context_config_t *context,enum STATE state) 
{
	context->state = state;
}

/*
 * API for get value
 */

int read_config(const char *filename) 
{
	return _read_config(filename);
}

static int _read_config(const char *filename) 
{
	int ret = 0;
	FILE *file = NULL;
	char buffer[_CONFIG_BUFFER];
	char token[_CONFIG_TOKEN];
	size_t pos = 0,len = 0;
	context_config_t context = {1,0,0,0,0,STATE_START,NULL,NULL,NULL,NULL}; // start state

	memset(buffer,'\0',_CONFIG_BUFFER);
	memset(buffer,'\0',_CONFIG_TOKEN);

	context.buffer = buffer;
	context.token = token;
	/*
	 * read file into buffer
	 * get_token
	 * go into the automation recycle
	 */

	if(!(file = fopen(filename,"r")))
		return E_CONF_FILE_NOT_EXIST;
	
	memset(buffer,'\0',_CONFIG_BUFFER);
	
	if((len = fread(buffer,sizeof(char),_CONFIG_BUFFER,file)) < sizeof(char) && ferror(file))  {
		return E_CONF_FILE_READ;
	}

	context.len = len;
	context.file = file;

	_parse_state(&context);

	fclose(file);
	
	return 0;
}

static int _parse_state(context_config_t *context)
{
	const char *token = context->token;
	unsigned int *symbol = &context->symbol;
	int ret;
	// get token
	// get symbol
	// parse state and router
	ret = get_token(context);
	if(ret != E_CONF_NO_ERROR)
		return ret;

	// it's difficult to make sure which variable save by recursion
	/*
	 * NFA:
	 *  S -->  aSb | aPb
	 *  P --> PP* | k=v
	 *  a --> token {
	 *  b --> }
	 *  k --> token
	 *  v --> token
	 */
	switch(context->state) {
	case STATE_START:
		if(!token || ! __istoken(context->token))
			return E_CONF_ILLEGAL_CHAR;
		goto __on_module_start;
	_module_start:
	case MODULE_START: {  // state S
		if(!token || ! __istoken(context->token))
			return E_CONF_ILLEGAL_CHAR;
		
		// it's import to differ which state route to MOUDLE_START or SYMBOL_KEY
		// check code in here get next char to check next state

		__on_module_start:
		if(*symbol == 0 || *symbol != _SYMBOL_LEFT_CHAR)
			return E_CONF_ILLEGAL_CHAR;
  		on_module_start(context);
		goto _module_end;

		__on_symbol_key_start:
		goto _symbol_key;
	}
		break;
	_module_end:
	case MODULE_END:
		if(token || *symbol != _SYMBOL_RIGHT_CHAR)
			return E_CONF_ILLEGAL_CHAR;
		on_module_end(context);
		break;
	case BLOCK_START:
		break;
	case BLOCK_END:
		break;
	case SYMBOL_KEY:
		break;
	case SYMBOL_VALUE:
		break;
    case SYMBOL_COMMENT:
		break;
	case STATE_END:
		break;
	}
	return 0;
}

static int get_token(context_config_t *context)
{
	const char *buffer = context->buffer;
	size_t *pos = &context->pos;
	char *token = context->token;
	enum STATE state = context->state;

	const char *p = buffer + *pos;
	
	if(*pos >= context->len) {
			
		if((context->len = fread(context->buffer,sizeof(char),_CONFIG_BUFFER,context-<file)) < sizeof(char) && ferror(file))  {
			return E_CONF_FILE_READ;
		}

		if(feof(context->file))
			return E_CONF_FILE_END;

		context->pos = 0;
		p = buffer;
	}

	while(*p != '\0') {

		fdebug_debug("current char:%c,pos:%d,buffer:%c.\n",*p,*pos,buffer[*pos]);

		/*
		 * check state routing by special character
		 */
		if(__isnewline(*p))  {

			if(state == SYMBOL_VALUE) 
				return E_CONF_ILLEGAL_CHAR;

			context->line++;		
		} else if(__iscomment(*p)) {
			// ignore automatically,don't use automation
			on_symbol_comment(context);
			p = buffer + *pos; // state has reset
			continue;
		} else if(__issymbol(*p)) { // special symbol name { or }
			*token = '\0';
			context->symbol = *p;
			return 0;
		} else if(__isspace(*p)) {  // ignore space
			p++;
			continue; // check next char
		} else if(__issplit(*p)) {  // get a var
			*token = '\0';
			context->symbol = *p;
			return 0;
		}

		// comment character
		*token++ = *p++;
		context->col++;
		(*pos)++;
	}
}

static int on_module_start(context_config_t *context)
{
	size_t *pos = &context->pos;
	char *token = context->token;
	unsigned int *symbol = &context->symbol;

	fdebug_test("on_module_start.\n");
	// check character then manage it.
	
	fdebug_test("on module start pos:%d,row:%d,col:%d.token:%s,current:%c.\n",*pos,context->line,context->col,context->token,*(context->buffer+*pos));
	fdebug_test("on_module_start module name:%s\n",context->token);
	
	__reset(token,symbol);

	_parse_state(context);

	return 0;
}

static int on_module_end(context_config_t *context)
{
	fdebug_test("on module end.\n");
	return -1;
}

static int on_block_start(context_config_t *context)
{
	printf("on block start.\n");
	return -1;
}

static int on_block_end(context_config_t *context)
{
	printf("on block end.\n");
	return -1;
}

static int on_symbol_key(context_config_t *context)
{
	// check previous state
	// get key for this module
	fdebug_test("on_symbol_key:%s.\n",context->token);
	// goto next state now
	set_context_state(context,SYMBOL_VALUE);
	return 0;
}

static int on_symbol_value(context_config_t *context)
{
	// check previous state
	// get value for last key
	printf("on_symbol_value:%s.\n",context->token);
	return -1;
}

/*
 * comment can't affect the automation's state,ignore in here
 */
static int on_symbol_comment(context_config_t *context)
{
	size_t *pos = &context->pos;

	fdebug_test("on_symbol_comment,line:%d,col:%d\n",context->line,context->col);
	// skip this line until a newline
	if(__skip_buffer(context->buffer,pos,_NEWLINE_CHAR) < 0) {
		fdebug_test("fail to skip buffer:%s.\n",context->msg);
		return -1;
	}

	context->line++;
	context->col = 0;

	fdebug_test("skip buffer:%d,buffer:%s.\n",*pos,context->buffer+*pos);
	return 0;
}

static int __skip_buffer(const char *buffer,size_t *pos, unsigned int c)
{
	size_t s = *pos;
	const char *p = buffer + s;
	//printf("now pos:%d,p:%s\n",*pos,p);
	while(*p != '\0' && *p++ != c) {
		//putchar(*p);
		s++;
	}

	//printf("buffer:%s\n",p);
	if(*(p-1) == c) {
		*pos = s + 1; // skip this char c itself
		return 0;
	}

	return -1;
}
