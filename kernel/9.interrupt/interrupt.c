#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/ioport.h>
#include <linux/stacktrace.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/timer.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/gpio.h>
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h> 
#include <linux/of_irq.h> 
#include <linux/irqreturn.h>


/*设备资源结构体*/
struct device_resource{
dev_t class21_dev;
struct cdev class21_cdev;
struct class *class_class21;
char read_kbuf[64];
char write_kbuf[64];
struct device_node *device_node;
struct property *node_property;
unsigned int *gpio5_beep;
int size;
u32 out_values[2];
int key_num;
int beep_num;
};
int irq;
struct tasklet_struct key_test_1;

struct device_resource dev1;
static void timer_function(unsigned long data);
DEFINE_TIMER(test_timer,timer_function,0,0);
static void timer_function(unsigned long data)
{
	int value;
	value = !gpio_get_value(dev1.beep_num);
	gpio_set_value(dev1.beep_num,value);
}


void test( unsigned long data)
{
	test_timer .expires = jiffies + msecs_to_jiffies(20);
	//定时器注册到内核里面
	add_timer(&test_timer);
	int i = data;
	printk("i is %d \n", i);
	while (i--)
	printk("test_key is %d \n", i);
}

irqreturn_t key_test(int irq,void *args)
{
	printk("test_key trigger off!\n");
	tasklet_schedule(&key_test_1);	
	return IRQ_HANDLED;                                                                                                                                                                                                               
}


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
	printk("kbuf is %d\n",write_res->write_kbuf[0]);
	//gpio_set_value(dev1.beep_num,write_res->write_kbuf[0]);
	/*
	if(write_res->write_kbuf[0]==1){
		*dev1.gpio5_beep |= (1<<1);
		printk("beep ON!!!\n");
	}
	else if(write_res->write_kbuf[0]==0){
		*dev1.gpio5_beep &= ~(1<<1);
		printk("beep OFF!!!\n");
	}
	*/
	return 0;
}
int class21_release(struct inode *inode, struct file *file){
	printk("release\n");
	return 0;
}
/*file_operation就是把系统调用和驱动程序关联起来的关键数据*/
struct file_operations class21_fops={
	.owner=THIS_MODULE,
	.read=class21_read,
	.write=class21_write,
	.open=class21_open,
	.release=class21_release,
};
	
int class21_probe(struct platform_device *pdev){
	printk("class21_probe ok\n");
/********************************直接获取节点********************************/
	printk("pdev_node_name is %s\n",pdev->dev.of_node->name);

	
/**************************通过函数获取**************************************/
	dev1.device_node=of_find_node_by_path("/class21_beep");
	//dump_stack();
	if(dev1.device_node ==NULL){
	printk("of_find_node_by_path is error\n");
	return 0;
	}
	printk("of_find_node_by_path name is %s\n",dev1.device_node->name);
/**************************查询属性的of函数***********************************/
		/*****************查找查找指定的属�?***********************/
	dev1.node_property=of_find_property(dev1.device_node, "compatible", &dev1.size);
	if(dev1.node_property ==NULL){
	
		printk("of_find_property is error\n");
		return 0;
	
	}
	printk("of_find_property name is %s\n",dev1.node_property->name);
	printk("of_find_property name is %s\n",(char *)dev1.node_property->value);
/************************读取u32类型的reg�?******   ***************************/
	of_property_read_u32_array(dev1.device_node,"reg",dev1.out_values, 2);
		
	printk("of_property_read_u32_array value is 0x%x\n",dev1.out_values[0]);
	printk("of_property_read_u32_array value is 0x%x\n",dev1.out_values[1]);
/******************************内存映射**************************************/
	dev1.gpio5_beep=of_iomap(dev1.device_node,0);
	if(dev1.gpio5_beep ==NULL){
		printk("of_iomap is error\n");
		return 0;	
	}
	printk("of_iomap is ok\n");
/****************************动态注册设备号**********************************/	
	if(alloc_chrdev_region(&(dev1.class21_dev), 0, 1, "hanmeimei")<0){
		printk("register chardev error!!!\n"); 
		return 0;
	}
/******************************注册字符设备*********************************/	
    /*dev1*/
	printk("MAJOR is %d\n",MINOR(dev1.class21_dev));
	printk("MINOR is %d\n",MINOR(dev1.class21_dev));
	dev1.class21_cdev.owner=THIS_MODULE;
	cdev_init(&(dev1.class21_cdev),&class21_fops);
	cdev_add(&(dev1.class21_cdev),dev1.class21_dev,1);
	dev1.class_class21=class_create(THIS_MODULE, "class21_beep_dev1");
	device_create(dev1.class_class21, NULL,dev1.class21_dev, NULL,"class21_dev1");
	printk("hello dev1!!!\n");
/****************************pinctrl与gpio子系统***********************/
	dev1.beep_num=of_get_named_gpio(dev1.device_node,"gpios",0);
	if(dev1.beep_num<0){
		printk("get named error!!!\n");
		return 0;
	}
	printk("named_gpio=%d\n",dev1.beep_num);
	if(gpio_request(dev1.beep_num,"gpio_beep")!=0){
		printk("gpio_request error!!!\n");
		return 0;
	}
	if(gpio_direction_output(dev1.beep_num,0)<0){
		printk("gspio_direction_output error!!!\n");
		goto free;
	}
	init_timer(&test_timer);
	
/****************************interrupt**************************/
	/*获得gpio编号*/
	dev1.key_num=of_get_named_gpio(dev1.device_node,"gpios",1);
	if(dev1.key_num<0){
		printk("get named error!!!\n");
		return 0;
	}
	gpio_direction_input(dev1.key_num);//gpio设置为
	irq=irq_of_parse_and_map(dev1.device_node,0);
	printk("irq is %d\n", irq);
	/*申请中断*/
	if(request_irq(irq,key_test,IRQF_TRIGGER_RISING,"key_irq",NULL)){
		printk("request_irq is error!\n");
		return 0;
	}
	tasklet_init(&key_test_1,test,66);
	return 0;
	free:
			gpio_free(dev1.beep_num);
			return 0;

}


int class21_remove(struct platform_device *pdev){
	printk("class21_remove ok\n");
	return 0;
}

const struct of_device_id driver_of_match_table[]={
	{.compatible="beep_test"},
	{}
};

/*匹配device*/
const struct platform_device_id class21_id_table={
	.name="class21_platform",
};

/*platform驱动结*/
struct platform_driver class21_driver={
	.probe=class21_probe,
	.remove=class21_remove,
	.driver={
		.name="class21_platform",
		.owner=THIS_MODULE,
		.of_match_table=driver_of_match_table,
	},
	.id_table=&class21_id_table,

};


static int class21_platforom_init(void){
	int rec;
	rec=platform_driver_register(&class21_driver);
	if(rec<0){
		printk("register error\n");
		return 0;
	}
	printk("driver init ok\n");
	return 0;
}

static void class21_platform_exit(void){
	del_timer(&test_timer);
	free_irq(irq,NULL);
	gpio_free(dev1.beep_num);
	tasklet_kill(&key_test_1);
	platform_driver_unregister(&class21_driver);
	iounmap(dev1.gpio5_beep);
	unregister_chrdev_region(dev1.class21_dev, 1);
	cdev_del(&(dev1.class21_cdev));
	device_destroy(dev1.class_class21, dev1.class21_dev);
	class_destroy(dev1.class_class21);
	printk("driver exit\n");
}

module_init(class21_platforom_init);
module_exit(class21_platform_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("mfk");

