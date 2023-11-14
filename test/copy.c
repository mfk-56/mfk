#include<stdio.h>
#include<string.h>
#include <libgen.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc,char **argv)
{
	char c;
	struct stat s_buf;
	stat(argv[2],&s_buf);
	if(S_ISDIR(s_buf.st_mode))
	{
		strcat(argv[2],"/");
		strcat(argv[2],basename(argv[1]));
	}
	if(argc != 3)
	{
		printf("<usage>:%s[file_path_1] [file_path_2]\n",argv[0]);
			
	}
	FILE *fp1=fopen(argv[1],"rb");
	if(fp1==NULL)
	{
		perror("fopen failed");
		return -1;
	}
	char *str=argv[2];
	FILE *fp2=fopen(str,"wb");
	if(fp2==NULL)
	{
		perror("fopen failed");
		return -1;
	}
	

	while(1)
	{
		c=fgetc(fp1);
		fputc(c,fp2);
		if(c==EOF)
		{
			break;
		}
	}
	fclose(fp1);
	fclose(fp2);
}
