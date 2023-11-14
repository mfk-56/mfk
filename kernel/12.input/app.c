#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
int main(int argc, char *argv[])
{
	int fd;
//���������¼��ṹ��
	struct input_event test_event;
//���豸�ڵ�
	fd = open("/dev/input/event1", O_RDWR);
	if (fd < 0)
	{
//���豸�ڵ�ʧ��
		perror("open error \n");
		return fd;
	}
	while (1)
	{
// ��ȡ�����¼�
	read(fd, &test_event, sizeof(test_event));
// ��������¼�����Ϊ�����¼������ӡ�¼������¼����ֵ
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
