#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/io.h>


#define GPIO5_DR 0x20AC000
dev_t class21_dev;
struct cdev class21_cdev;
struct class *class_class21;
//struct device *class21_dev;
unsigned int * gpio5_beep;

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
		printk("eroor!!!\n");
		return 0;
	}
	printk("ioremap ok!!!\n");
	return 0;

}

static void misctest_exit(void){

	unregister_chrdev_region(class21_dev, 1);
	cdev_del(&class21_cdev);
	device_destroy(class_class21, class21_dev);
	class_destroy(class_class21);
	printk("byebye!!!\n");

}



module_init(misctest_init);
module_exit(misctest_exit);
MODULE_LICENSE("GPL");


