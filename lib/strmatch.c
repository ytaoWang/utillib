#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "strmatch.h"

static unsigned long power(int radix,int num);
static long stoi(const char *txt,int begin,int from,int radix);

/**
 * naive method for string match
 * text[s,...,s+m-1] = p[0,...,m-1]
 * n = s + m -1
 * s = n - m +1
 */
const char* naive_match(const char *text,const char *pattern)
{
    int m,n,s;
    
    m = strlen(pattern);
    n = strlen(text);

    assert(n >=m);
    for(s = 0;s <= n-m;++s)
        if(!strncmp(text+s,pattern,m))
            return text + s;
    
    return NULL;
}

/*
 *  Rabin-Karp string match 
 * Ts = t[s+1 .... s + m]
 * Ts+1 = t[s+1+1 .... s+1+m]
 * Ts+1 = d * (ts - d^(m-1) *t[s+1]) + t[s+1+m]
 * model = Ts(mod q)
 */
#define RK_N 65537

static const char* __rk_match(const char *text,const char *pattern,int d)
{
    long dst,model,h;
    int m,n,i;    

    n = strlen(text);
    m = strlen(pattern);
    assert(n >=m);
    h = power(d,m-1);
    
    //preprocessing
    dst = stoi(text,0,m,d) % RK_N;
    model = stoi(pattern,0,m,d) % RK_N;

    for(i = 0;i <= n-m;++i) {
        if(dst == model) {
            if(!strncmp(text+i,pattern,m)) 
                return text + i;
        }
        dst = (d*(dst - h * stoi(text,i,1,d)) + stoi(text,i+m,1,d)) % RK_N;
        //printf("i:%d,text:%c,i+m+1:%d,text:%c\n",i,*(text+i),i+m+1,*(text+i+m));
    }
    
    return NULL;
}

const char* rk_match(const char *text,const char *pattern)
{
    return __rk_match(text,pattern,10);
}

const char* dfa_match(const char *text,const char *pattern)
{
    return NULL;
}

/**
  * Knuth-Morris-pratt string Match algorithm
  * finite state transition function
  * definition next[q] = max{k:k<q Pk ] Pq} and next[q] < q
  * next[k]:the largest length suffix of Pq
  * t[s+1,...,s+q] = p[1,...,q],t[s'+1,...,s'+k] = p[1,...,k]
  * s + q = s' + k,s' = s+q-k
  * 
  */
const char* kmp_match(const char *text,const char *pattern)
{
    int n,m,q,k,i;
    int *next = NULL;
    
    n = strlen(text);
    m = strlen(pattern);
    assert(n >= m);
    
    next = calloc(m+1,sizeof(int));
    //preprocessing state transition function
    q = 0;
    k = -1;
    //make a flag
    /*
     * Given next[1],next[2],....,next[q],how can we compute next[q+1]?
     * if p[q+1] == p[next[q]+1],then next[q+1] = next[q]+1
     * else next[q+1] = next[next[q]] + 1,p should slide to a place such that the prefix of p[1..next[q]] 
     * occurs as a suffix of p[q-next[q+1].. q];this information is stored in next[next[q]]
     */
    next[0] = k;
    for(i = 1;i < m;i++) {
        while(k >= 0 && pattern[k+1] != pattern[i])
            k = next[k];
        if(pattern[k+1] == pattern[i])
            k++;
        next[i] = k;
    }
    
    //match string
    k=-1;
    for(i = 0;i < n;i++) {
        while(k >=0 && pattern[k+1] != text[i])
            k = next[k];
        if(pattern[k+1] == text[i])
            k++;
        if(k==m-1) {
            free(next);
            return text+i-m+1;
        }
    }
    
    
    free(next);
    return NULL;
}

static unsigned long __power(int radix,int num)
{
    unsigned long value = 0;

    if(num == 1)
        return radix;
    
    if(num %2) {
        value = __power(radix,(num-1)/2);
        return radix * value * value;
    } else {
        value = __power(radix,num/2);
        return value * value;
    }
    
}


static unsigned long power(int radix,int num)
{   
    return __power(radix,num);
}

static long stoi(const char *txt,int begin,int length,int radix)
{
    const char *p;
    unsigned int minus = 0;
    long value = 0;
    
    p = txt + begin;
    if(*p == '-') {
        minus = 1;
        p++;
    }

    while(p < txt + begin + length) {
        value = value * radix + *p - '0';
        //printf("%c(%d),%s\n",*p,*p,p);
        p++;
    }

    if(minus)
        value = -value;
    return value;
}
