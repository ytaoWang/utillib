#include <stdio.h>

#include "strmatch.h"

int main(int argc,char *argv[])
{
    char text[] = "bacbababababcabab";
    char pattern[] = "ababababca";
    const char *t=NULL;
    long value;
    
    if((t = naive_match(text,pattern)))
        puts(t);
    
    if((t = kmp_match(text,pattern)))
        puts(t);
    
    if((t = naive_match("1234556","56")))
        puts(t);

    if((t = rk_match("1234556","56")))
        puts(t);
    if((t = kmp_match("1234556","56")))
        puts(t);
    /*
    value = stoi("-12",0,3,10);
    printf("%ld\n",value);
    */
    return 0;
}
