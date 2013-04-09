#ifndef __ATOMIC_H_
#define __ATOMIC_H_
/*
	author:datao0907
	email:datao0907@163.com
	this is from kernel:/include/asm-x86/atomic_32.h
*/
typedef int atomic_t;

static inline atomic_t fetch_and_add(atomic_t *value,atomic_t key)
{
  asm volatile("lock;addl %1,%0"
               :"+m"(*value)
               :"ir"(key)
               :"memory");
  return *value;
}

static inline atomic_t fetch_and_sub(atomic_t *value,atomic_t key)
{
  asm volatile("lock;subl %1,%0"
               :"+m"(*value)
               :"ir"(key)
               :"memory");
  return *value - key;
}

static inline atomic_t add_and_fetch(atomic_t *value,atomic_t key)
{
  asm volatile("lock;addl %1,%0"
               :"+m"(*value)
               :"ir"(key)
               :"memory");
  return *value + key;
}

static inline  atomic_t sub_and_fetch(atomic_t *value,atomic_t key)
{
  asm volatile("lock;subl %1,%0"
               :"+m"(*value)
               :"ir"(key)
               :"memory");
  return *value - key;
}

static inline atomic_t sub_and_test(atomic_t *value,atomic_t key)
{
  int c;
  asm volatile("lock;subl %2,%0;sete %1"
               :"+m"(*value),"=qm"(c)
               :"ir"(key)
               );
  return c;
}

#endif
