#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/ioport.h>

struct resource class21_resource[]={
[0]={
	.start=0x20AC000,
	.end=0x20AC003,
	.name="GPIO5_DR",
	.flags=IORESOURCE_MEM,
	},


};

void class21_device_release(struct device *dev){

	printk("class21_device_release ok\n");
}


struct platform_device class21_device={
	.name="class21_platform",//platform 设备的名字，用来和 platform 驱动相匹配，名字相同才能匹配成功
	.id=1,				//ID 是用来区分如果设备名字相同的时候
	.dev={				//内置的 device 结构体
		.release=class21_device_release,
			},
	.num_resources=ARRAY_SIZE(class21_resource),//资源结构体数量。ARRAY_SIZE，用来判断一个数组的元素个数
	.resource=class21_resource,///指向一个资源结构体数组。一般包含设备信息。

};



static int class21_init(void){
	int rec;
	rec=platform_device_register(&class21_device);
	if(rec<0){
		printk("error\n");
		return 0;
	}
	printk("init ok\n");
	return 0;

}

static void class21_exit(void){
	platform_device_unregister(&class21_device);
	printk("exit\n");

}

module_init(class21_init);
module_exit(class21_exit);

MODULE_LICENSE("GPL");

