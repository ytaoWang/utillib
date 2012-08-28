#ifndef __STR_MATCH_H_
#define __STR_MATCH_H_

/*
 * simplicity string match
 * find pattern string in text
 */
const char* naive_match(const char *text,const char *pattern);
const char* rk_match(const char *text,const char *pattern);
//const char* dfa_match(const char *text,const char *pattern);
//long stoi(const char *txt,int begin,int length,int radix);
const char* kmp_match(const char *text,const char *pattern);

#endif
