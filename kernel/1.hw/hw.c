#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

ssize_t class1008_read(struct file *file , char __user *ubuf, size_t size, loff_t *loff_t){
	char kbuf[]="I am lihua";
	if(copy_to_user(ubuf, kbuf, strlen(kbuf))!=0){
		printk("error");
		return 0;
	}
	return 0;
}
ssize_t class1008_write(struct file *file , const char __user *ubuf, size_t size, loff_t *loff_t){
	char kbuf[64]={};
	if(copy_from_user(kbuf, ubuf, size)!=0){
		printk("error");
		return 0;		
	}
	printk("kbuf is %d",kbuf[0]);
	return 0; 
}

int class1008_open(struct inode *inode , struct file *file){

	printk("file open!!!\n");
	return 0;
}
int class1008_release(struct inode *inode , struct file *file){
	printk("file close!!!\n");
	return 0;
}

struct file_operations class1008={	//�ļ���������
	.owner=THIS_MODULE,				//ָ����ǰģ��
	.read=class1008_read,
	.write=class1008_write,
	.open=class1008_open,
	.release=class1008_release,
};


struct miscdevice classtest={		//�����豸�ṹ��
	.minor=MISC_DYNAMIC_MINOR,		//���豸��
	.name="class1008",				//�豸�ڵ����ƣ��ڵ��ڡ�dev��
	.fops=&class1008,				//���ļ���������

};


static int hw_init(void){	//������ں���
	int class;
	
	class=misc_register(&classtest);		//���ں�ע�������豸
	if(class<0){
		printk("error");
		return 0;
	}
	printk("helloworld!!!\n");
	return 0;

}


static void hw_exit(void){	//�������ں���

	misc_deregister(&classtest);	//���ں�ע����ɾ���������豸	
	printk("byebye!!!\n");
}


module_init(hw_init);	//ģ���ע����ں���
module_exit(hw_exit);	//ģ���ע����ں���
MODULE_LICENSE("GPL");
MODULE_AUTHOR("she_cd");


