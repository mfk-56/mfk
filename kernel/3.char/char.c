#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>


struct cdev class21_cdev;
dev_t class21_dev;
struct class * class21_class;




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
	printk("%d\n",kbuf[0]);
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


 struct file_operations class21_char={   //文件操作集合
	 .owner=THIS_MODULE,			//指定当前模块
  	 .read=class21_read,
  	 .write=class21_write,
  	 .open=class21_open,
  	 .release=class21_release,
 };

 struct miscdevice classtest={		//杂项设备结构体
	 .minor=MISC_DYNAMIC_MINOR, 	//次设备号
	 .name="class21_char", 			 	//设备节点名称，节点在、dev下
	 .fops=&class21_char,				//绑定文件操作集合
 	 
 };



 static int helloworld_init(void)
 {
 	int class;
	class21_dev=MKDEV(120, 100);
//	class=register_chrdev_region(class21_dev, 1,"mfk");//静态注册设备号
//	if(class<0){
//		printk("registe chardev error!\n");
//		return -1;
//	}
	
	class=alloc_chrdev_region(&class21_dev, 0, 1, "MFK");//动态注册设备号
		if(class<0){
			printk("register chardev error!!!\n");
			return -1;
		}
	printk("MAJOR is %d\n",MAJOR(class21_dev));
	class21_cdev.owner=THIS_MODULE;
	cdev_init(&class21_cdev, &class21_char); //初始化字符设备，将字符设备与文件建立联系
	cdev_add(&class21_cdev, class21_dev, 1);//在Linux系统中添加字符设备
	class21_class=class_create(THIS_MODULE, "class21_char");//注册类（class）	
	device_create(class21_class, NULL,class21_dev, NULL, "class21_device");//创建设备节点
	
 	printk("hello world!!\n");
	return 0;
 }
 static void helloworld_exit(void)
 {
 	device_destroy(class21_class, class21_dev);//删除设备节点
	class_destroy(class21_class);//删除类（class） 
	unregister_chrdev_region(class21_dev, 1);//注销设备号
	cdev_del(&class21_cdev);   //删除字符设备
 	printk("byebye!!\n");
 }

module_init(helloworld_init);
module_exit(helloworld_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MFK");
