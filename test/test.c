#include<stdio.h>
#include <stdlib.h>

int main(int argc,char **argv)
{
	int i=0;
	for(i=0;i<argc;i++)
	{
		printf("%d\n",atoi(argv[i]));
	}
	return 0;
}
