/**
 * read config file using C
 * config style:
   
   module1 {
   	name = jack # this is comment
	sex = male # female
	age = 18  # age
   }
   
   module2 {
   	name = kitty
	sex = female
	age = 36
   }
 */


#include <stdio.h>
#include <stdlib.h>

enum STATE {
    MODULE_START = 0,
	MODULE_END,
	SYMBOL_KEY_START,
	SYMBOL_KEY_END,
	SYMBOL_VALUE_START,
	SYMBOL_VALUE_END,
	SYMBOL_COMMENT_START,
	SYMBOL_COMMENT_END
};


typedef struct {
    unsigned int line;
    unsigned int col;
    enum STATE state;
    char *msg;  
} context_config_t;


int read_config(const char *filename,int *row,int *col);

int main(int argc, char *argv[]) 
{
  
   if( argc < 2 ) {
	   fprintf(stderr,"usage:%s file.\n",argv[0]);
	   return -1;
   }
   /*
	* check file exist or not
	* analyze config file
	*/
   
   return 0;
}


int read_config(const char *filename,int *row,int *col) 
{
    /*
	 * read file into buffer
	 * get_token
	 * go into the automation recycle
	 */

  
  return 0;
}
