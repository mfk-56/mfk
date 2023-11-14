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

int main(int argc, char **argv){

	key_t  key_value;		//键值
	int    shmid;			//共享内存标识符
	void  *p_shm = NULL;	//共享内存段的首地址
	off_t  file_size;		//存储文件大小
	char  *file_name = NULL;//文件名称
	void  *tmp = NULL;		//在共享内存段中偏移 用来写入数据
	FILE  *fp  = NULL;
	size_t write_len;
	int    i;

	if( -1 == (key_value = ftok("/home",100)) ){
		perror("ftok");
		return -1;
	}
	if( -1 == (shmid = shmget(key_value,PAGE_SIZE * 256 * 1024,IPC_CREAT | 0666)) ){	/* 获取共享内存标识符 */
		perror("shmget");
		return -1;
	}

	/* 接入目标共享内存段 获取内存的首地址 */
	if( (void *)-1 == (p_shm = shmat(shmid, NULL, 0)) ){
		perror("shmat");
		return -1;
	}
	printf("=== Attached shared memory succeed ===\n\n");

	tmp       = p_shm;
	file_size = *(off_t *)tmp;	//获取文件大小
	tmp += sizeof(off_t);
	file_name = (char *)tmp;	//获取文件名
	tmp += 256;
	printf("file size : %ld\n", file_size);
	printf("file name : %s\n", file_name);

	if( NULL == (fp = fopen(file_name,"wb")) ){
		perror("fopen");
		return -1;
	}
	/* 文件数据写入 */
	for( i = 0; i < (file_size / PAGE_SIZE); i++ ){
		write_len = fwrite(tmp, 1, PAGE_SIZE, fp);
		tmp += write_len;
		printf("%03d : %lu bytes read.\n", i + 1, write_len);
	}
	write_len = fwrite(tmp, 1, file_size % PAGE_SIZE, fp);
	printf("%03d : %lu bytes read.\n", i + 1, write_len);

	/* 与目标共享内存段断开连接 */
	shmdt(p_shm);
	printf("\n=== Detached shared memory succeed ===\n");
	fclose(fp);

	return 0;
}

