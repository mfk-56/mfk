#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "mysem.h"
#include <semaphore.h>


#define PAGE_SIZE	4096
int   shmid;			//共享内存标识符

int main(int argc, char **argv){

	key_t key_value;		//键值
	void *p_shm = NULL;		//共享内存段的首地址
	int   semid;

	/* 获取并初始化信号量 */
	semid = semArrInit(2);
	if( -1 == (key_value = ftok("/home",222)) ){
		perror("ftok");
		return -1;
	}
	/* 获取共享内存标识符 */
	if( -1 == (shmid = shmget(key_value,PAGE_SIZE, IPC_CREAT | 0666)) ){
		perror("shmget");
		return -1;
	}

	/* 接入目标共享内存段 获取内存的首地址 */
	if( (void *)-1 == (p_shm = shmat(shmid, NULL, 0)) ){
		perror("shmat");
		return -1;
	}
	printf("=== Attached shared memory succeed ===\n\n");

	/* 数据读写 */
	/* 信号量内部的值不能小于0,如果一个程序对信号量进行资源申请,企图将值-1,但此时其值已经为0,那么程序会阻塞,直到其值>0 */
	while(1)
	{	
		/* 申请信号量资源(-1) */
		semP(semid, 1);
		printf("%s\n",(char *)p_shm);
		
	}
	
	/* 信号量使用完毕后,应当立即释放(删除信号量) */
	if( -1 == semctl(semid, 0, IPC_RMID) )
	{
		perror("semctl");
		return -1;
	}
	
	/* 与目标共享内存段断开连接 */
	if( -1 == shmdt(p_shm) ){
		perror("shmdt");
		return -1;
	}
	printf("\n=== Detached shared memory succeed ===\n");

	return 0;
}


