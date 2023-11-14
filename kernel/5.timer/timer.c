#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/timer.h>


#define GPIO5_DR 0x20AC000




dev_t class21_dev;
struct cdev class21_cdev;
struct class *class_class21;
//struct device *class21_dev;
unsigned int * gpio5_beep;


struct timer_list test_timer;

static void timer_function(unsigned long data);
static void timer_function(unsigned long data)
{
	/**
	*修改定时值，如果定时器还没有激活的话，mod_timer 函数会激活定时器
	*/
	static int n=0;
	n++;
	printk("n=%d\n",n);
	//if(n<=4){
		if(n%2==1){
			*gpio5_beep |= (1<<1);
			printk("beep ON!!!\n");
		}
		if(n%2==0){
			*gpio5_beep &= ~(1<<1);
			printk("beep OFF!!!\n");
		}
	mod_timer(&test_timer, jiffies + 2 * HZ);
	//}
}


ssize_t class21_read (struct file *file, char __user *ubuf, size_t size, loff_t * loff_t){
	char kbuf[]="class21_kbuf";
	

	if(copy_to_user(ubuf, kbuf, strlen(kbuf)) !=0){

	printk("error");
	return 0;
	}
	
	printk("read\n");
	return 0;
}
ssize_t class21_write(struct file *file, const char __user *ubuf, size_t size, loff_t *loff_t){

	char kbuf[64]={};

	if(copy_from_user(kbuf,ubuf, size) !=0){
		printk("error");
			return 0;


	}

	printk("write\n");
	printk("write_kbuf is %d\n",kbuf[0]);


	if(kbuf[0]==1){

		*gpio5_beep |= (1<<1);
		printk("beep ON!!!\n");

	}else if(kbuf[0]==0){

		*gpio5_beep &= ~(1<<1);
		printk("beep OFF!!!\n");

	}

	return 0;


}
int class21_open (struct inode * inode, struct file *file){
	printk("open\n");
	return 0;


}
int class21_release(struct inode *inode, struct file *file){
	printk("release\n");
	return 0;



}


struct file_operations class21_fops={
	.owner=THIS_MODULE,
	.read=class21_read,
	.write=class21_write,
	.open=class21_open,
	.release=class21_release,

};




static int misctest_init(void){
	//int rec;
/*
	class21_dev=MKDEV(150, 25);
	rec=register_chrdev_region(class21_dev,1,"class21_dev_t");
	if(rec<0){
		printk("error");
		return 0;
	}
	*/
	printk("hello world!!!\n");
	alloc_chrdev_region(&class21_dev, 0, 1, "alloc_class21");
	

	class21_cdev.owner=THIS_MODULE;
	cdev_init(&class21_cdev,&class21_fops);
	cdev_add(&class21_cdev,class21_dev,1);

	class_class21=class_create(THIS_MODULE, "class21_beep");
	device_create(class_class21, NULL, class21_dev, NULL,"class21_dev");
	gpio5_beep=ioremap(GPIO5_DR, 4);
	if(gpio5_beep==NULL){
		printk("error!!!\n");
		return 0;
	}
	printk("ioremap ok!!!\n");
	init_timer(&test_timer);
	test_timer.function=timer_function;
	//初始化 test_timer.expires 意为超时时间
	test_timer .expires = jiffies + 2* HZ;
	//定时器注册到内核里面，启动定时器
	add_timer(&test_timer);
	return 0;

}

static void misctest_exit(void){
	del_timer_sync(&test_timer);
	printk("timer_exit is ok\n");
	unregister_chrdev_region(class21_dev, 1);
	cdev_del(&class21_cdev);
	device_destroy(class_class21, class21_dev);
	class_destroy(class_class21);
	printk("byebye!!!\n");

}



module_init(misctest_init);
module_exit(misctest_exit);
MODULE_LICENSE("GPL");


