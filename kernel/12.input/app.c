#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
int main(int argc, char *argv[])
{
	int fd;
//定义输入事件结构体
	struct input_event test_event;
//打开设备节点
	fd = open("/dev/input/event1", O_RDWR);
	if (fd < 0)
	{
//打开设备节点失败
		perror("open error \n");
		return fd;
	}
	while (1)
	{
// 读取输入事件
	read(fd, &test_event, sizeof(test_event));
// 如果输入事件类型为按键事件，则打印事件类型事件码和值
	if (test_event .type == EV_KEY)
	{
		printf("type is %#x \n", test_event.type);
		printf("code is %#x \n", test_event.code);
		printf("value is %#x \n", test_event.value);
	}
	}
close(fd);
return 0;
}
