/*
* @Author:topeet
* @Description:使用输入子系统设计按键驱动
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>

#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/timer.h>
//添加输入子系统的头文件
#include <linux/input.h>


static void timer_function(unsigned long data);
DEFINE_TIMER(test_timer,timer_function,0,0);
struct device_node *test_device_node;
struct property *test_node_property;
//定义一个输入设备 test_dev
struct input_dev *test_dev; //定义一个输入设备 test_dev
int irq;
int gpio_nu;
/**
* @description:超时处理函数
* @param {*}
* @return {*}
*/
static void timer_function(unsigned long data)
{
	int value;
	value = !gpio_get_value(gpio_nu);
	input_report_key(test_dev, KEY_1, value); //上报事件
	input_sync(test_dev);
}



	//中断处理函数
irqreturn_t test_key(int irq, void *args)
{
	printk("test_key\n");
	test_timer .expires = jiffies + msecs_to_jiffies(20);
	//定时器注册到内核里面
	add_timer(&test_timer);
	return IRQ_RETVAL(IRQ_HANDLED);
}
	/**
	* @brief beep_probe : 与设备信息层 (设备树) 匹配成功后自动执行此函数，
	* @param inode : 文件索引
	* @param file  : 文件
	* @return 成功返回 0
	*/
int beep_probe(struct platform_device *pdev)
{
	int ret = 0;
	printk("beep_probe\n");
	//of_find_node_by_path 函数通过路径查找节点，/test_key 是设备树下的节点路径
	test_device_node = of_find_node_by_path("/class21_beep");
	if (test_device_node == NULL)
	{
		printk("of_find_node_by_path is error \n");
		return -1;
	}
	//of_get_named_gpio 函数获取 GPIO 编号
	gpio_nu = of_get_named_gpio(test_device_node, "gpios", 0);
	if (gpio_nu < 0)
	{
		printk("of_get_namd_gpio is error \n");
		return -1;
	}
	// 设置 GPIO 为输入模式
	gpio_direction_input(gpio_nu);
	//irq = gpio_to_irq(gpio_nu);
	// 获取中断号
	irq = irq_of_parse_and_map(test_device_node, 0);

	printk("irq is %d \n", irq);
	/*申请中断，irq:中断号名字
	test_key：中断处理函数
	IRQF_TRIGGER_RISING：中断标志，意为上升沿触发
	"test_key"：中断的名字
	*/
	ret = request_irq(irq, test_key, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "test_key", NULL);
	if (ret < 0)
	{
		printk("request_irq is error \n");
		return -1;
	}


	
	//申请一个 input_dev 输入设备
	test_dev = input_allocate_device();
	// 设置 input_dev 名字
	test_dev->name = "test_key";
	// 设置事件和事件值
	// 设置产生按键事件
	__set_bit(EV_KEY, test_dev->evbit);
	//设置产生哪些按键值，表示这个设备要支持 key_1
	__set_bit(KEY_1, test_dev->keybit);
	//向 Linux 内核注册 input_dev
	ret = input_register_device(test_dev);
	if (ret < 0)
	{
		printk("input_register_device is error \n");
		goto error_input_register;
	}
	return 0;
	error_input_register:
	input_unregister_device(test_dev);
}
	
int beep_remove(struct platform_device *pdev)
{
	printk("beep_remove\n");
	return 0;
}
const struct platform_device_id beep_idtable = {
	.name = "beep_test",
};
const struct of_device_id of_match_table_test[] = {
	{.compatible = "class21_platform_"},
	{},
};
struct platform_driver beep_driver = {
	//3. 在 beep_driver 结构体中完成了 beep_probe 和 beep_remove
	.probe = beep_probe,
	.remove = beep_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "class21_platform_",
		.of_match_table = of_match_table_test,
	},
	//4 .id_table 的优先级要比 driver.name 的优先级要高，优先与 .id_table 进行匹配
	.id_table = &beep_idtable
};
	
static int beep_driver_init(void)
{
	// 1.我们看驱动文件要从 init 函数开始看
	int ret = 0;
	//2. 在 init 函数里面注册了 platform_driver
	ret = platform_driver_register(&beep_driver);
	if (ret < 0)
	{
		printk("platform_driver_register error \n");
	}
	printk("platform_driver_register ok \n");
	
	return 0;
}
	
static void beep_driver_exit(void)
{
	del_timer(&test_timer);
	free_irq(irq, NULL);
	input_unregister_device(test_dev);
	platform_driver_unregister(&beep_driver);
	printk("gooodbye! \n");
}
module_init(beep_driver_init);
module_exit(beep_driver_exit);	
MODULE_LICENSE("GPL");

