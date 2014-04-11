/**
 * read config file using C
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


// define state operation
#define STATE_FUNCTION_NAME(name) (on##name)

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
    unsigned int line;
    unsigned int col;
    enum STATE state; // now state
	enum STATE last; // last state
	char *parent; // current token parent's token
    char *msg;  
	char *token; // current token
} context_config_t;

typedef int (*config_func_t)(context_config_t *context,size_t *pos);

typedef struct {
	enum STATE state;
	config_func_t func;
} context_state_operation;

/*
 * define error number for debugging
 */

static unsigned int errorno_config;

static int _read_config(const char *filename,int *row,int *col);
static void get_token(const char *buffer,size_t *pos,char *token,context_config_t *context);

// state operation
static int on_module_start(context_config_t *context,size_t *pos);
static int on_module_end(context_config_t *context,size_t *pos);
static int on_block_start(context_config_t *context,size_t *pos);
static int on_block_end(context_config_t *context,size_t *pos);
static int on_symbol_key(context_config_t *context,size_t *pos);
static int on_symbol_value(context_config_t *context,size_t *pos);
static int on_symbol_comment(context_config_t *context,size_t *pos);

static int __skip_buffer(const char *buffer,size_t *pos, int c);


static context_state_operation state_func[] = {
	{ STATE_START,NULL },
	{ MODULE_START, on_module_start },
	{ MODULE_END, on_module_end },
	{ BLOCK_START, on_block_start },
	{ BLOCK_END, on_block_end },
	{ SYMBOL_KEY, on_symbol_key },
	{ SYMBOL_VALUE, on_symbol_value },
	{ SYMBOL_COMMENT, on_symbol_comment },
	{ STATE_END, NULL }
};

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

static int inline __issymbol(int c)
{
	return c == _SYMBOL_LEFT_CHAR || c == _SYMBOL_RIGHT_CHAR;
}

static void set_context_state(context_config_t *context,enum STATE state) 
{
	context->last = context->state;
	context->state = state;
}

/*
 * API for get value
 */

int read_config(const char *filename,int *row,int *col) 
{
	return _read_config(filename,row,col);
}

static int _read_config(const char *filename,int *row,int *col) 
{
	int ret = 0;
	FILE *file = NULL;
	char buffer[_CONFIG_BUFFER];
	char token[_CONFIG_TOKEN];
	size_t pos = 0,len = 0;
	context_config_t context = {1,0,MODULE_START,STATE_START,NULL,NULL,NULL}; // start state

	context.msg = buffer;
	context.token = token;
	/*
	 * read file into buffer
	 * get_token
	 * go into the automation recycle
	 */

	if(!(file = fopen(filename,"r")))
		return -1;
	
	memset(buffer,'\0',_CONFIG_BUFFER);
	
	if((len = fread(buffer,sizeof(char),_CONFIG_BUFFER,file)) == 0)
		return -1;

	while((context.state != MODULE_END)) {

		memset(token,'\0',_CONFIG_TOKEN); // reset token
		printf("in context state read_config,pos:%d,len:%d\n",pos,len);
		get_token(buffer,&pos,token,&context);
		if((ret = state_func[context.state].func(&context,&pos)) < 0)
			return ret;
		getchar();
		if(pos == len) {  // buffer has finish
			len = fread(buffer,sizeof(char),_CONFIG_BUFFER,file);
			pos = 0;
		}
	}


	fclose(file);
	
	return 0;
}

static void get_token(const char *buffer,size_t *pos,char *token,context_config_t *context)
{
	const char *p = buffer + *pos;
	
	assert(p != NULL);

	while(*p != '\0') {
		context->col++;
		(*pos)++;

		printf("current char:%c,pos:%d,buffer:%c.\n",*p,*pos,buffer[*pos]);

		/*
		 * check state routing by special character
		 */
		if(__isnewline(*p))  {
			assert( context->state == MODULE_START || context->state == MODULE_END || \
					context->state == SYMBOL_COMMENT || context->state == SYMBOL_VALUE);

			context->line++;		
		} else if(__iscomment(*p)) {
			if(*(context->token) != '\0') { // organize a new token,must use original token,or problem in here
				(*pos)--;
				return;
			}
			set_context_state(context,SYMBOL_COMMENT);
			*token++ = *p;
			return;
		} else if(__issymbol(*p)) { // special symbol name { or }
			// *token++ = *p;      // lets SYMBOL_* state check status and take some actions
			return;
		} else if(__isspace(*p)) {  // ignore space
			p++;
			continue; // check next char
		} else if(__issplit(*p)) {  // get a var
			if(context->state != SYMBOL_COMMENT)  { // comment,ignore this state
				set_context_state(context,SYMBOL_KEY);
				return;
			}
		}

		// comment character
		*token++ = *p++;
	}
}

static int on_module_start(context_config_t *context,size_t *pos)
{
	printf("on module start pos:%d,row:%d,col:%d.token:%s,current:%c.\n",*pos,context->line,context->col,context->token,*(context->msg+*pos));
	printf("on_module_start module name:%s\n",context->token);
	getchar();
	return 0;
}

static int on_module_end(context_config_t *context,size_t *pos)
{
	printf("on module end.\n");
	return -1;
}

static int on_block_start(context_config_t *context,size_t *pos)
{
	printf("on block start.\n");
	return -1;
}

static int on_block_end(context_config_t *context,size_t *pos)
{
	printf("on block end.\n");
	return -1;
}

static int on_symbol_key(context_config_t *context,size_t *pos)
{
	// check previous state
	// get key for this module
	printf("on_symbol_key:%s.\n",context->token);
	// goto next state now
	set_context_state(context,SYMBOL_VALUE);
	return 0;
}

static int on_symbol_value(context_config_t *context,size_t *pos)
{
	// check previous state
	// get value for last key
	printf("on_symbol_value:%s.\n",context->token);
	return -1;
}

static int on_symbol_comment(context_config_t *context,size_t *pos)
{
	printf("on_symbol_comment,line:%d,col:%d\n",context->line,context->col);
	// skip this line until a newline
	if(__skip_buffer(context->msg,pos,_NEWLINE_CHAR) < 0) {
		printf("fail to skip buffer:%s.\n",context->msg);
		return -1;
	}

	context->line++;
	context->col = 0;
	//restore previous state
	set_context_state(context,context->last);

	printf("skip buffer:%d,buffer:%s.\n",*pos,context->msg+*pos);
	return 0;
}

static int __skip_buffer(const char *buffer,size_t *pos, int c)
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
		*pos = s + 1; // skip this char itself
		return 0;
	}

	return -1;
}
