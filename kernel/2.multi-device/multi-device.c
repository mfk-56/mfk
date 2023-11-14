#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/io.h>


#define GPIO5_DR 0x20AC000
#define GPIO1_IO3_DR 0X20E0068
//struct device *class21_dev;


struct device_resource{
dev_t class21_dev;
struct cdev class21_cdev;
struct class *class_class21;
char read_kbuf[64];
char write_kbuf[64];
unsigned int * gpio5_beep;
unsigned int * gpio1_3_led2;

};
struct device_resource dev1;
struct device_resource dev2;

static int class21_open (struct inode * inode, struct file *file){
	//file->private_data=&dev1;
	file->private_data=container_of(inode->i_cdev,struct device_resource,class21_cdev);
	printk("open\n");
	return 0;


}

ssize_t class21_read (struct file *file, char __user *ubuf, size_t size, loff_t * loff_t){
	struct device_resource *read_res=(struct device_resource *)file->private_data;
	read_res->read_kbuf[0]="hello world!";
	if(copy_to_user(ubuf, read_res->read_kbuf, strlen(read_res->read_kbuf))!=0){
	printk("error\n");
	return 0;
	}	
	printk("read\n");
	return 0;
}
ssize_t class21_write(struct file *file, const char __user *ubuf, size_t size, loff_t *loff_t){

	struct device_resource *write_res=(struct device_resource*)file->private_data;
	if(copy_from_user(write_res->write_kbuf, ubuf, size)!=0){
		printk("error\n");
		return 0;
	}

	printk("write\n");
	printk("kbuf is %d",write_res->write_kbuf[0]);
	if(write_res->write_kbuf[0]==1){

		*dev1.gpio5_beep |= (1<<1);
		printk("beep ON!!!\n");

	}else if(write_res->write_kbuf[0]==0){

		*dev1.gpio5_beep &= ~(1<<1);
		printk("beep OFF!!!\n");

	}
	else if(write_res->write_kbuf[0]==3){

		*dev2.gpio1_3_led2 &= ~(1<<1);
		printk("led ON!!!\n");

	}
	else if(write_res->write_kbuf[0]==4){

		*dev2.gpio1_3_led2 |= (1<<1);
		printk("led OFF!!!\n");

	}

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
	int class;
	class=alloc_chrdev_region(&(dev1.class21_dev), 0, 2, "hanmeimei");
	if(class<0){
		printk("register chardev error!!!\n"); 
		return 0;
	}
    /*dev1*/
	printk("MAJOR is %d\n",MINOR(dev1.class21_dev));
	printk("MINOR is %d\n",MINOR(dev1.class21_dev));
	dev1.class21_cdev.owner=THIS_MODULE;
	cdev_init(&(dev1.class21_cdev),&class21_fops);
	cdev_add(&(dev1.class21_cdev),dev1.class21_dev,1);
	dev1.class_class21=class_create(THIS_MODULE, "class21_beep_dev1");
	device_create(dev1.class_class21, NULL,dev1.class21_dev, NULL,"class21_dev1");
	printk("hello dev1!!!\n");
	/*dev2*/
	dev2.class21_dev=dev1.class21_dev+1;
	printk("MAJOR is %d\n",MINOR(dev2.class21_dev));
	printk("MINOR is %d\n",MINOR(dev2.class21_dev));
	dev2.class21_cdev.owner=THIS_MODULE;
	cdev_init(&(dev2.class21_cdev),&class21_fops);
	cdev_add(&(dev2.class21_cdev),dev2.class21_dev,1);
	dev2.class_class21=class_create(THIS_MODULE, "class21_beep_dev2");
	device_create(dev2.class_class21, NULL,dev2.class21_dev, NULL,"class21_dev1");
	printk("hello dev2!!!\n");

	dev1.gpio5_beep=ioremap(GPIO5_DR, 4);
	if(dev1.gpio5_beep==NULL){
		printk("error!!!\n");
		return 0;
	}
	dev2.gpio1_3_led2=ioremap(GPIO1_IO3_DR, 4);
	if(dev2.gpio1_3_led2==NULL){
		printk("error!!!\n");
		return 0;
	}
	printk("ioremap ok!!!\n");
	return 0;

}

static void misctest_exit(void){

	unregister_chrdev_region(dev1.class21_dev, 1);
	cdev_del(&(dev1.class21_cdev));
	device_destroy(dev1.class_class21, dev1.class21_dev);
	class_destroy(dev1.class_class21);

	unregister_chrdev_region(dev2.class21_dev, 1);
	cdev_del(&(dev2.class21_cdev));
	device_destroy(dev2.class_class21, dev2.class21_dev);
	class_destroy(dev2.class_class21);
	printk("byebye!!!\n");

}



module_init(misctest_init);
module_exit(misctest_exit);
MODULE_LICENSE("GPL");


