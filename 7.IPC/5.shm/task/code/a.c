#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <libgen.h>

#define PAGE_SIZE	4096

static off_t getFileSize(const char *pathname);	//获取文件大小

int main(int argc, char **argv){

	key_t  key_value;		//键值
	int    shmid;			//共享内存标识符
	void  *p_shm = NULL;		//共享内存段的首地址
	off_t  file_size;		//存储文件大小
	char  *file_name = NULL;	//文件名称
	void  *tmp = NULL;		//在共享内存段中偏移 用来写入数据
	FILE  *fp  = NULL;
	size_t readlen;
	int    i;

	if( argc != 2 ){
		printf("<usage> : %s [file pathname]\n",argv[0]);
		return -1;
	}

	if( -1 == (key_value = ftok("/home",100)) ){
		perror("ftok");
		return -1;
	}
	if( -1 == (shmid = shmget(key_value,PAGE_SIZE * 256 * 1024,IPC_CREAT | 0666)) ){	/* 获取共享内存标识符 */
		perror("shmget");
		return -1;
	}
	if( NULL == (fp = fopen(argv[1],"rb")) ){
		perror("fopen");
		return -1;
	}

	/* 接入目标共享内存段 获取内存的首地址 */
	if( (void *)-1 == (p_shm = shmat(shmid, NULL, 0)) ){
		perror("shmat");
		return -1;
	}
	printf("=== Attached shared memory succeed ===\n\n");

	file_size = getFileSize(argv[1]);	//获取文件大小
	file_name = basename(argv[1]);		//获取文件名
	tmp       = p_shm;
	*(off_t *)tmp = file_size;	//前8字节写入文件大小
	tmp += sizeof(off_t);
	strcpy((char *)tmp, file_name);	//随后的256字节写入文件名称
	tmp += 256;
	
	/* 文件数据写入 */
	for( i = 0; i < (file_size / PAGE_SIZE); i++ ){
		readlen = fread(tmp, 1, PAGE_SIZE, fp);
		tmp += readlen;
		printf("%03d : %lu bytes written.\n", i + 1, readlen);
	}
	readlen = fread(tmp, 1, file_size % PAGE_SIZE, fp);
	printf("%03d : %lu bytes written.\n", i + 1, readlen);

	/* 与目标共享内存段断开连接 */
	shmdt(p_shm);
	printf("\n=== Detached shared memory succeed ===\n");
	fclose(fp);

	return 0;
}

//获取文件大小
static off_t getFileSize(const char *pathname){
	struct stat file_stat;
	if( -1 == stat(pathname,&file_stat) ){
		perror("stat");
		return -1;
	}
	return file_stat.st_size;
}

