#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

void *udprecv_Message(void *arg);

int main(int argc,char **argv){

	if(argc!=4){
		printf("<usage> : %s [ipv4] [port] [local_port]\n", argv[0]);
		return -1;
	}
	int udp_socket;
	struct sockaddr_in udp_addr;   	//目标
	struct sockaddr_in local_addr;
	char buffer[1024]  ={0};
	pthread_t pt;
	int optval = 1;
	if( -1 == (udp_socket = socket(AF_INET, SOCK_DGRAM,0)) ){
		perror("socket");
		return -1;
	}

	memset(&udp_addr, 0x00, sizeof(udp_addr));
	udp_addr.sin_family      = AF_INET;
	udp_addr.sin_port        = htons(atoi(argv[2]));
	udp_addr.sin_addr.s_addr = inet_addr(argv[1]);

	memset(&local_addr, 0x00, sizeof(local_addr));
	local_addr.sin_family      = AF_INET;
	local_addr.sin_port        = htons(atoi(argv[3]));
	local_addr.sin_addr.s_addr = INADDR_ANY;
	if( setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ){
		perror("setsockopt");
		return -1;
	}
	if( bind(udp_socket, (const struct sockaddr *)&local_addr, sizeof(local_addr)) ){
		perror("bind");
		return -1;
	}
	if( pthread_create(&pt, NULL, udprecv_Message, &udp_socket) ){
		perror("pthread_create");
		return -1;
	}

	system("clear");
	printf("==== UDP local-port : %d ====\n\n", atoi(argv[3]));
	
	while(1){
		memset(buffer, 0x00, sizeof(buffer));
		fgets(buffer, sizeof(buffer), stdin);
		if(sendto(udp_socket,buffer,sizeof(buffer),0,(const struct sockaddr*)&udp_addr, sizeof(udp_addr))<0){
			perror("sendto");
			break;
		}
	}
	close(udp_socket);
	return 0;
}
void *udprecv_Message(void *arg){
	char buffer[1024]={0};
	int sockfd = *(int *)arg;
	struct sockaddr_in udp_addr;
	socklen_t addr_len = sizeof(udp_addr);
	while(1){
		memset(buffer, 0x00, sizeof(buffer));
		if(recvfrom(sockfd, buffer, sizeof(buffer), 0,(struct sockaddr *)&udp_addr, &addr_len)<0){
			perror("recvfrom");
			break;
		}
		printf("[%s:%d] recv: %s\n", inet_ntoa(udp_addr.sin_addr), ntohs(udp_addr.sin_port),buffer);
		
	}
	close(sockfd);
	_exit(-1);
	return NULL;
}
