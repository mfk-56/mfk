#ifndef _CONTACTS_H
#define _CONTACTS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>



#define SAVE_DATA_PATH	"./data/savedata"
#define ERROR_HANDLE(func)	fprintf(stderr,"#Error: File\"%s\"-line(%d)-%s:%s\n" \
							,__FILE__,__LINE__,func,strerror(errno));

/* 联系�? contacts 结构体类�? */
struct contacts
{
	char name[16];
	char phone_number[12];
	struct contacts *next;
};
typedef struct contacts T_CONT;

/* 函数声明 */
void _SIGINT_handler(int signum);
void *linkListInit(T_CONT **pphead);
void linkListDestory(T_CONT *phead);
void insertNode(T_CONT *phead);//插入
void deleteNode(T_CONT *phead);
void printfNode(T_CONT *phead);
void changeNode(T_CONT *phead);
void searchNode(T_CONT *phead);

#endif /* _CONTACTS_H */
