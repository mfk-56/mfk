#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

void *threadHandler(void *arg);

/* 全局变量 */
char buffer[1024];
int client_socket;


int main(int argc,char **argv){
	
	if( argc != 3 ){
		printf("<usage> : %s [ipv4] [port]\n", argv[0]);
		return -1;
	}
	struct sockaddr_in target;	//目标的internet协议地址结构
	if(-1==(client_socket=socket(AF_INET, SOCK_STREAM, 0))){
		perror("socket");
		return 0;
	}
	memset(&target,0x00,sizeof(target));
	target.sin_family      = AF_INET;
	target.sin_port        = htons(atoi(argv[2]));
	target.sin_addr.s_addr = inet_addr(argv[1]);
	if(connect(client_socket,(const struct sockaddr*)&target,sizeof(target))){
		perror("connect");
		return 0;
	}
	pthread_t pt;
	if( pthread_create(&pt, NULL, threadHandler, NULL) ){
		perror("pthread_create");
		return 0;
	}
	while(1){
//		if(read(client_socket,buffer,sizeof(buffer))<0){
//			perror("read");
//			break;
//		}
		if(read(client_socket,buffer,sizeof(buffer))==0){
			printf("Connection closed by foreign host.\n");
			return 0;
		}
		printf("recive:%s\n",buffer);
		memset(buffer,0x00,sizeof(buffer));
	}
	close(client_socket);
	return 0;
}

/* 线程处理函数 */
void *threadHandler(void *arg){
	while(1){
		fgets(buffer, sizeof(buffer), stdin);
		if(write(client_socket,buffer,sizeof(buffer))<0){
			perror("write");
			break;
		}
	memset(buffer,0x00,sizeof(buffer));
	}
}

