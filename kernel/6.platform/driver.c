#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/ioport.h>
#include <linux/stacktrace.h>
#include <linux/io.h>
#include <linux/timer.h>

unsigned int * gpio5_beep;

struct resource *class21_resource; 
struct timer_list test_timer;

static void timer_function(unsigned long data);
static void timer_function(unsigned long data)
{
	/**
	*�޸Ķ�ʱֵ�������ʱ����û�м���Ļ���mod_timer �����ἤ�ʱ��
	*/
	static int n=0;
	n++;
	printk("n=%d\n",n);
	if(n<=4){
		if(n%2==1){
			*gpio5_beep |= (1<<1);
			printk("beep ON!!!\n");
		}
		if(n%2==0){
			*gpio5_beep &= ~(1<<1);
			printk("beep OFF!!!\n");
		}
	mod_timer(&test_timer, jiffies + 2 * HZ);
	}
	
}


int class21_probe(struct platform_device *pdev){
	printk("class21_probe ok\n");
	
/**********************ͨ������ֱ�ӻ�ȡ***************************/
	printk("device_resource_name is %s\n",pdev->resource[0].name);
	printk("get_resource start is 0x%x\n",pdev->resource[0].start);
	printk("get_resource end is 0x%x\n",pdev->resource[0].end);
	printk("platform_device__name is %s\n",pdev->name);
	

printk("*****************************************\n");
/**************************ͨ��������ȡ**************************************/
	class21_resource=platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(class21_resource==NULL){
		printk("get resource error\n");
		return 0;
	}

	
	printk("get_resource name is %s\n",class21_resource->name);
	printk("get_resource start is 0x%x\n",class21_resource->start);
	printk("get_resource end is 0x%x\n",class21_resource->end);

	//dump_stack();
	
	gpio5_beep=ioremap(class21_resource->start, 4);
	if(gpio5_beep==NULL){
		printk("error!!!\n");
		return 0;
	}
	printk("ioremap ok!!!\n");
	init_timer(&test_timer);
	test_timer.function=timer_function;
	//��ʼ�� test_timer.expires ��Ϊ��ʱʱ��
	test_timer .expires = jiffies + 2* HZ;
	//��ʱ��ע�ᵽ�ں����棬������ʱ��
	add_timer(&test_timer);
	return 0;

	
}
int class21_remove(struct platform_device *pdev){

	printk("class21_remove ok\n");
	return 0;

}

const struct platform_device_id class21_id_table={
	.name="class21_platform",

};


struct platform_driver class21_driver={
	.probe=class21_probe,
	.remove=class21_remove,
	.driver={
		.name="class21_platform",
		.owner=THIS_MODULE

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
	del_timer_sync(&test_timer);
	printk("timer_exit is ok\n");
	platform_driver_unregister(&class21_driver);
	printk("driver exit\n");
	
}

module_init(class21_platforom_init);
module_exit(class21_platform_exit);

MODULE_LICENSE("GPL");

