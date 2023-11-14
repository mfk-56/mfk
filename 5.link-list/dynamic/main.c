#include <stdio.h>
#include <stdlib.h>
#include "contacts.h"

int displayMenu();



T_CONT *phead = NULL;


int main()
{

	int choose;
	
	signal(SIGINT,_SIGINT_handler);

	if( !linkListInit(&phead) )
	{	//链表初始化
		fprintf(stderr,"#Error : linklist init failed.\n");
		return -1;
	}

	while(1)
	{
		choose = displayMenu();
		
		switch( choose )
		{
			case 1 : insertNode(phead);break;
			case 2 : deleteNode(phead);break;
			case 3 : changeNode(phead);break;
			case 4 : searchNode(phead);break;
			case 5 : printfNode(phead);break;
			case 6 : linkListDestory(phead);return 0;
			default :{
				printf("### Invalid option argument.\n");
				printf("### Press Enter to continue.\n");
				getchar();
				break;}
		}
	}

	return 0;
}

int displayMenu()
{
	int a;
	system("clear");	//清屏
	printf("=============================================================\n");
	printf("1.Create a new contact\n");
	printf("2.Delete a contact \n");
	printf("3.change contact\n");
	printf("4.Search contact\n");
	printf("5.Display Contacts\n");
	printf("6.Exit \n");
	printf("=============================================================\n");
	printf("Input choose:");
	scanf("%d%*c",&a);
	return a;
		
}


/*

栈上的内存,生命周期在定义时开始,结束在使用完成,内存由系统释放
堆上的内存,生命周期在malloc开始,结束在free释放

	void *malloc(size_t size);
	函数功能:申请动态内存
	函数参数:size	   	 要申请的内存大小(bytes)
	返 回 值:成功返回指针,指向已经分配好的内存;失败,返回NULL

	void free(void *ptr);
	函数功能:释放动态内存
	函数参数:ptr		 指向要释放的内存的首地址
	返 回 值:无返回值

	//在堆上申请 4 字节的内存 用于存放一个整型值
	int *p = (int *)malloc(sizeof(int));
	if( p == NULL ){
		perror("malloc");
		return -1;
	}
	*p = 123;
	printf("*p = %d\n",*p);
	free(p);
*/
