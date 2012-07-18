#ifndef __LIST_H_
#define __LIST_H_ 
#ifdef __cplusplus
extern "C" {
#include <cstdio>
 
#else
	#include <stdio.h>
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE*)0)->MEMBER)
#endif

#ifndef CONTAINER_OF
#define CONTAINER_OF
#define container_of(ptr,type,member) ({	\
 const typeof( ((type *)0)->member) *__mptr = (ptr); \
 (type *)( (char*)__mptr - offsetof(type,member) );})
#endif


struct list {
struct list *next;
struct list *prev;
};

static inline void list_init(struct list *head)
{
	head -> next = head;
	head -> prev = head;
}
static inline void _list_add(struct list *head,struct list *_list)
{
	struct list *next = head->next;

//	printf("head->next:%p,%d in list_add\n",head->next,head->next==NULL);
//	printf("_list->prev:%p,%d in list_add\n",_list->prev,_list->prev==NULL);
//	fflush(stdout);
	
//	if(head->next != head)
	{
	 next ->prev = _list;
	 _list->next = next;
	_list->prev = head;
	head->next = _list;
/*
	_list->next = next;
	_list->prev = head;
	
 	next->prev = _list;
	head->next = _list;
*/	}
/*
	else {
		head->next = _list;
		head->prev = _list;
		_list->next = head;
		_list->prev = head;
	}
*/
//	printf("after _list_add head:%p,head->next:%p,_list:%p,%d\n",\
		head,head->next,_list,head->next==_list);
//	fflush(stdout);
}

//在head后插入链表_list
static inline void list_add_head(struct list *head,struct list *list)
{
	_list_add(head,list);
}
//在head前面插入链表 list
static inline void list_add_tail(struct list *head,struct list *list)
{
//	printf("in list_add_tail head:%p,prev:%p,%d\n",\
		head,head->prev,head->prev == head);
//	fflush(stdout);
	_list_add(head->prev,list);
}

static inline void _list_del(struct list* head,struct list *_list)
{
	if(head == _list ) return;
	
	struct list* next = _list->next;
	next->prev = head;
	head->next = next;	
}

//删除head后面元素list
static inline void list_del(struct list *head,struct list *list)
{
	_list_del(head,list);
}

static inline void list_del_element(struct list* _list)
{
	struct list *p;
	
	p = _list->prev;

	if(p && p != _list)
		list_del(p,_list);
    list_init(_list);
}

static inline void list_move(struct list *dest,struct list *src)
{
	list_del_element(src);
	list_add_tail(dest,src);
}

static inline void list_move_head(struct list *dest,struct list *src)
{
	list_del_element(src);
	list_add_head(dest,src);
}

static inline int list_empty(struct list *head)
{
	return head->next == head;	
}

//链表遍历
/**
 *@ptr:  the struct pointer which struct list within it
 *@type: the struct type 
 *@member:the struct list member name belogs to struct pointer
 **/
#define list_entry(ptr,type,member) container_of(ptr,type,member)

#define list_for_each_entry(pos,head,member) \
	for(pos = list_entry((head)->next,typeof(*pos),member);\
		&pos->member != (head);\
		pos = list_entry(pos->member.next,typeof(*pos),member))

#define list_for_each_entry_safe(pos,ptr,type,member) \
	for(pos = list_entry((ptr)->next,type,member); \
	    &pos->member != ptr; pos =list_entry(pos->member.next,type,member))
/*
 * may be a little problem but nothing
 */
#define list_for_each_entry_del(pos,_pos,head,member)   \
	for(pos = list_entry((head)->next,typeof(*_pos),member),\
		 _pos = \
		list_entry(pos->member.next,typeof(*_pos),member);\
		&pos->member!=(head);\
	pos = _pos,_pos=list_entry(pos->member.next,typeof(*_pos),member))

#ifdef __cplusplus
}
#endif

#endif
