#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>
#define SEND_TYPE 1102
#define RECV_TYPE 1101

/* 消息结构 msgbuf */
struct msgbuf {
	long mtype;			/* 消息类型(非负) */
	char data[1024];		/* 消息数据(用户自定义) */
};

void *threadRecvMsg(void *arg)
{
	int msqid=*(int *)arg;
	struct msgbuf msg;
	while(1){
			if( -1 == msgrcv(msqid,&msg,sizeof(msg) - sizeof(long),RECV_TYPE,0) ){
				perror("msgrcv");
				return (void *)-1;
			}
			printf("recv : %s\n",msg.data);
		}
	return NULL;
}


/* 主函数 */
int main(int argc, char **argv){

	key_t key_value;	//键值
	int   msqid;		//消息队列标识符
	pthread_t pt;
	struct msgbuf msg;

	/* 创建键值 */
	if( -1 == (key_value = ftok("/home/meson",123)) ){
		perror("ftok");
		return -1;
	}
	/* 获取一个消息队列标识符 */
	if( -1 == (msqid = msgget(key_value, IPC_CREAT | 0666)) ){
		perror("msgget");
		return -1;
	}
	if(pthread_create(&pt,NULL,threadRecvMsg,&msqid))
	{
		perror("pthread_create");
		return -1;
	}
	/* 发送数据 */
	while(1){
		fgets(msg.data,sizeof(msg.data),stdin);
		msg.mtype = SEND_TYPE ;
		if( -1 == msgsnd(msqid,&msg,sizeof(msg) - sizeof(long),0) ){
			perror("msgsnd");
			return -1;
		}
	}
	
	return 0;
}
