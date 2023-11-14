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

struct file_operations class1008={	//文件操作集合
	.owner=THIS_MODULE,				//指定当前模块
	.read=class1008_read,
	.write=class1008_write,
	.open=class1008_open,
	.release=class1008_release,
};


struct miscdevice classtest={		//杂项设备结构体
	.minor=MISC_DYNAMIC_MINOR,		//次设备号
	.name="class1008",				//设备节点名称，节点在、dev下
	.fops=&class1008,				//绑定文件操作集合

};


static int hw_init(void){	//驱动入口函数
	int class;
	
	class=misc_register(&classtest);		//向内核注册杂项设备
	if(class<0){
		printk("error");
		return 0;
	}
	printk("helloworld!!!\n");
	return 0;

}


static void hw_exit(void){	//驱动出口函数

	misc_deregister(&classtest);	//向内核注销（删除）杂项设备	
	printk("byebye!!!\n");
}


module_init(hw_init);	//模块的注册入口函数
module_exit(hw_exit);	//模块的注册入口函数
MODULE_LICENSE("GPL");
MODULE_AUTHOR("she_cd");


