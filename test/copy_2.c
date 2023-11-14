/* 编写程序 复制一篇文件 */
/* <usage> : %s [src_file] [dest_path]  */

#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#define ERRMSG(action)	fprintf(stderr,"#Error : line %d \"%s\" : %s \n" \
								,__LINE__,action,strerror(errno));

int main(int argc,char **argv){

	if( argc != 3 ){
		printf("<usage> : %s [src_file] [dest_path]\n",argv[0]);
		return -1;
	}

	char *srcfile_path = argv[1];			//源文件完成路径
	char *srcfile_name = basename(argv[1]);	//源文件文件名
	char *destfile     = argv[2];			//参数2 目标路径(携带文件名)
	FILE *fp_1 = NULL;
	FILE *fp_2 = NULL;
	char str[1024]={0};
	struct stat file_stat;	//定义stat类型的结构体变量 用于存放文件信息

	if( (-1 == stat(destfile,&file_stat)) ){	// 读取destfile文件信息
		if( errno != ENOENT ){
			return -1;
		}
	}else{
		if( S_ISDIR(file_stat.st_mode) ){	//判断destfile是否是一个目录
			/* 是路径 拼接文件名 */
			strcat(destfile,"/");
			strcat(destfile,srcfile_name);
			printf("line(%d) : destfile = %s\n",__LINE__,destfile);
		}
	}

	fp_1 = fopen(srcfile_path,"rb");
	if( !fp_1 ){
		ERRMSG("fopen srcfile_path");
		return -1;
	}
	
	fp_2 = fopen(destfile,"wb");
	if( !fp_2 ){
		perror("fopen destfile");
		return -1;
	}
	fread(str,1,1024,fp_1);
	fwrite(str,1,1024,fp_2);

	fclose(fp_1);
	fclose(fp_2);
	
	return 0;
}