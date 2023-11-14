#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>

struct cdev class1008_cdev;
dev_t class1008_dev;
struct class * class1008_class;


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




static int hw_init(void){	//驱动入口函数
	int class;
	/*
	class1008_dev=MKDEV(120, 100);
	class=register_chrdev_region(class1008_dev, 1,"lihua");
	if(class<0){
		printk("register chardev error!!!\n");
		return 0;
	}
	*/
	class=alloc_chrdev_region(&class1008_dev, 0, 1, "hanmeimei");
	if(class<0){
		printk("register chardev error!!!\n");
		return 0;
	}

	printk("MAJOR is %d\n",MAJOR(class1008_dev));
	printk("MAJOR is %d\n",MINOR(class1008_dev));
	class1008_cdev.owner=THIS_MODULE;
	cdev_init(&class1008_cdev, &class1008);
	cdev_add(&class1008_cdev, class1008_dev, 1);

	class1008_class=class_create(THIS_MODULE, "class1008");

	device_create(class1008_class, NULL,class1008_dev, NULL, "class1008_device");
	printk("helloworld!!!\n");
	return 0;

}


static void hw_exit(void){	//驱动出口函数
	device_destroy(class1008_class, class1008_dev);
	class_destroy(class1008_class);
	unregister_chrdev_region(class1008_dev, 1);
	cdev_del(&class1008_cdev);

	printk("byebye!!!\n");
}


module_init(hw_init);	//模块的注册入口函数
module_exit(hw_exit);	//模块的注册入口函数
MODULE_LICENSE("GPL");
MODULE_AUTHOR("she_cd");


