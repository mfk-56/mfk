#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

 ssize_t class21_read (struct file *file, char __user *ubuf, size_t size, loff_t *loff_t)
{
	char kbuf[64]="mfk";
	if(copy_to_user(ubuf, kbuf, strlen(kbuf))!=0){
		printk("read error\n");
		return -1;
	}
	return 0;
}
 ssize_t class21_write (struct file *file, const char __user *ubuf, size_t size, loff_t *loff_t)
 {
 	char kbuf[64]={};	
 	if(copy_from_user(kbuf, ubuf, size)!=0){
		printk("write error\n");
		return -1;
	}
	printk("%d\n",ubuf[0]);
	return 0;
 }
 int class21_open (struct inode *inode, struct file *file)
 {
 	printk("file open!!\n");
	return 0;
 }
 int class21_release (struct inode *inode, struct file *file)
 {
 	printk("file release!!\n");
	return 0;
 }


 struct file_operations class21={   //文件操作集合
	 .owner=THIS_MODULE,			//指定当前模块
  	 .read=class21_read,
  	 .write=class21_write,
  	 .open=class21_open,
  	 .release=class21_release,
 };

 struct miscdevice classtest={		//杂项设备结构体
	 .minor=MISC_DYNAMIC_MINOR, 	//次设备号
	 .name="class21_misc", 			 	//设备节点名称，节点在、dev下
	 .fops=&class21_misc,				//绑定文件操作集合
 	 
 };



 static int helloworld_init(void)
 {
 	int class;
	class=misc_register(&classtest);		//向内核注册杂项设备
	if(class<0){
		printk("error");
		return 0;
	}
 	printk("hello world!!\n");
	return 0;
 }
 static void helloworld_exit(void)
 {
 	misc_deregister(&classtest);				//向内核注销（删除）杂项设备	
 	printk("byebye!!\n");
 }

module_init(helloworld_init);
module_exit(helloworld_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MFK");
