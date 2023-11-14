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
#include<linux/kdev_t.h>
#include<linux/device.h>
#include <linux/io.h>
#include <linux/timer.h>
#include <linux/of.h>
#include <linux/of_address.h>

struct resource *class17_resource; 
struct timer_list test_timer;
struct device_resource dev1;

static void timer_function(unsigned long data);
/****************设备资源结构体************************************/
struct device_resource
	{
	int class;
	struct cdev class1008_cdev;
	dev_t class1008_dev;
	struct class * class1008_class;
	char read_kbuf[64];
	char write_kbuf[64];
	unsigned int * gpio5_beep;
	struct device_node *device_node;
	 u32  out_values[2];
	int size_test;
	struct property *node_property;
	};
	
ssize_t class1008read(struct file *file, char __user *ubuf, size_t size, loff_t *loff_t)
	{
	struct device_resource *read_res=(struct device_resource *)file->private_data;

	read_res->read_kbuf[0]="hello read";
      if(copy_to_user(ubuf,read_res->read_kbuf,strlen(read_res->read_kbuf))!=0)
      	{
		  printk("error");
		  return 0;
      	}
	  return 0;
	}
ssize_t class1008write(struct file *file, const char __user *ubuf, size_t size, loff_t *loff_t)
	{
	int ret;
	struct device_resource *write_res=(struct device_resource *)file->private_data;
	if(copy_from_user(write_res->write_kbuf, ubuf, size)!=0)
		{
		printk("error");
		return 0;
		}
		printk("write is %d",write_res->write_kbuf[0]);
		ret=write_res->write_kbuf[0];
		printk("write is %d",ret);

	
	return 0;
	}

int class1008open(struct inode *inode, struct file *file)
	{
		file->private_data=&dev1;

	printk("file open\n");
	return 0;
	}

int class1008release(struct inode *inode, struct file *file)
	{
	printk("file close\n");
	return 0;
	}

static void timer_function(unsigned long data)
{
	static u32 writebuf=(1<<1);
	writebuf=~writebuf;
	printk(" This is timer_function \n");
	/**
	*修改定时值，如果定时器还没有激活的话，mod_timer 函数会激活定时器
*/
	mod_timer(&test_timer, jiffies + 2 * HZ);
}

struct file_operations class1008={	//文件操作集合
	.owner=THIS_MODULE,				//指定当前模块
    .read=class1008read,
    .write=class1008write,
    .open=class1008open,
    .release=class1008release,
};

int class17_probe(struct platform_device *pdev){
	printk("class17_probe ok\n");

/*******************通过路径从设备树获得节点信息*****************************/
	dev1.device_node=of_find_node_by_path("/class1008_test");
	if(dev1.device_node==NULL){
		printk("find node is error!!!!\n");
		return 0;
	}
	printk("node name is %s\n",dev1.device_node->name);
/***********************通过函数从设备树获得节点属性**************************/

	if(of_property_read_u32_array(dev1.device_node,"reg",dev1.out_values,2)<0){

		printk("find reg is error!!!!\n");
		return 0;

	};

	printk("reg start 0x%x\n",dev1.out_values[0]);
	printk("reg start 0x%x\n",dev1.out_values[1]);

	dev1.node_property=of_find_property(dev1.device_node, "compatible", &dev1.size_test);
	if(dev1.node_property==NULL){
		printk("find node_property is error!!!!\n");
		return 0;
	}
	printk("node name is %s\n",dev1.node_property->name);
	printk("node name is %s\n",(char *)dev1.node_property->value);

	dev1.gpio5_beep=of_iomap(dev1.device_node,0);
	printk("beep map address is 0x%x\n",*dev1.gpio5_beep);



/**********************通过参数直接获取硬件资源信息***************************
	printk("device_resource_name is %s\n",pdev->resource[0].name);
	printk("get_resource start is 0x%x\n",pdev->resource[0].start);
	printk("get_resource end is 0x%x\n",pdev->resource[0].end);
	printk("platform_device__name is %s\n",pdev->name);
	printk("*****************************************\n");
************************通过函数获取硬件资源信息******************************
	class17_resource=platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(class17_resource==NULL){
		printk("get resource error\n");
		return 0;
	}
	printk("get_resource name is %s\n",class17_resource->name);
	printk("get_resource start is 0x%x\n",class17_resource->start);
	printk("get_resource end is 0x%x\n",class17_resource->end);
******************************动态获取设备号***********************************/
	dev1.class=alloc_chrdev_region(&(dev1.class1008_dev),0,1,"xiaoming");
	if(dev1.class<0)
	{
		printk(" register region error!!\n");
		return 0;
			
	}
/************注册字符设备、创建设备节点******************************************/
	printk(" MAJOR is :%d\n",MAJOR(dev1.class1008_dev));
	printk(" MINOR is :%d\n",MINOR(dev1.class1008_dev));
	dev1.class1008_cdev.owner=THIS_MODULE;
	cdev_init(&(dev1.class1008_cdev), &class1008);
	cdev_add(&(dev1.class1008_cdev),dev1.class1008_dev, 1);
	dev1.class1008_class=class_create(THIS_MODULE,"class1008");//创建类
	device_create(dev1.class1008_class, NULL, dev1.class1008_dev, NULL,"class1008_device");//创建节点

/************************蜂鸣器虚拟地址映射************************************/

/**********************内核定时器***********************************************/
	
	init_timer(&test_timer);
	test_timer.function=timer_function;
	//初始化 test_timer.expires 意为超时时间
	test_timer .expires = jiffies + 2* HZ;
	//定时器注册到内核里面，启动定时器
	add_timer(&test_timer);


/**********************打印当前函数的调用过程*********************************/

	dump_stack();

	return 0;	
}
int class17_remove(struct platform_device *pdev){

	printk("class17_remove ok\n");
	return 0;

}

const struct platform_device_id class17_id_table={
	.name="class17_platform",

};


const struct of_device_id	device_of_match_table[]={
	{.compatible="beep_test"},

};


struct platform_driver class17_driver={
	.probe=class17_probe,
	.remove=class17_remove,
	.driver={
		.name="class17_test",
		.owner=THIS_MODULE,
		.of_match_table=device_of_match_table,
	},
	.id_table=&class17_id_table,

};


static int class17_platforom_init(void){
	int rec;
	rec=platform_driver_register(&class17_driver);
	if(rec<0){
		printk("register error\n");
		return 0;

	}
	printk("driver init ok\n");

	return 0;

}

static void class17_platform_exit(void){
	platform_driver_unregister(&class17_driver);

	device_destroy(dev1.class1008_class,dev1.class1008_dev);
	class_destroy(dev1.class1008_class);
	unregister_chrdev_region(dev1.class1008_dev, 1);
	cdev_del(&(dev1.class1008_cdev));
	
	del_timer_sync(&test_timer);

	printk("driver exit\n");
	
}

module_init(class17_platforom_init);
module_exit(class17_platform_exit);

MODULE_LICENSE("GPL");

