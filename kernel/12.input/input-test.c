/*
* @Author:topeet
* @Description:ʹ��������ϵͳ��ư�������
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
//���������ϵͳ��ͷ�ļ�
#include <linux/input.h>


static void timer_function(unsigned long data);
DEFINE_TIMER(test_timer,timer_function,0,0);
struct device_node *test_device_node;
struct property *test_node_property;
//����һ�������豸 test_dev
struct input_dev *test_dev; //����һ�������豸 test_dev
int irq;
int gpio_nu;
/**
* @description:��ʱ������
* @param {*}
* @return {*}
*/
static void timer_function(unsigned long data)
{
	int value;
	value = !gpio_get_value(gpio_nu);
	input_report_key(test_dev, KEY_1, value); //�ϱ��¼�
	input_sync(test_dev);
}



	//�жϴ�����
irqreturn_t test_key(int irq, void *args)
{
	printk("test_key\n");
	test_timer .expires = jiffies + msecs_to_jiffies(20);
	//��ʱ��ע�ᵽ�ں�����
	add_timer(&test_timer);
	return IRQ_RETVAL(IRQ_HANDLED);
}
	/**
	* @brief beep_probe : ���豸��Ϣ�� (�豸��) ƥ��ɹ����Զ�ִ�д˺�����
	* @param inode : �ļ�����
	* @param file  : �ļ�
	* @return �ɹ����� 0
	*/
int beep_probe(struct platform_device *pdev)
{
	int ret = 0;
	printk("beep_probe\n");
	//of_find_node_by_path ����ͨ��·�����ҽڵ㣬/test_key ���豸���µĽڵ�·��
	test_device_node = of_find_node_by_path("/class21_beep");
	if (test_device_node == NULL)
	{
		printk("of_find_node_by_path is error \n");
		return -1;
	}
	//of_get_named_gpio ������ȡ GPIO ���
	gpio_nu = of_get_named_gpio(test_device_node, "gpios", 0);
	if (gpio_nu < 0)
	{
		printk("of_get_namd_gpio is error \n");
		return -1;
	}
	// ���� GPIO Ϊ����ģʽ
	gpio_direction_input(gpio_nu);
	//irq = gpio_to_irq(gpio_nu);
	// ��ȡ�жϺ�
	irq = irq_of_parse_and_map(test_device_node, 0);

	printk("irq is %d \n", irq);
	/*�����жϣ�irq:�жϺ�����
	test_key���жϴ�����
	IRQF_TRIGGER_RISING���жϱ�־����Ϊ�����ش���
	"test_key"���жϵ�����
	*/
	ret = request_irq(irq, test_key, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "test_key", NULL);
	if (ret < 0)
	{
		printk("request_irq is error \n");
		return -1;
	}


	
	//����һ�� input_dev �����豸
	test_dev = input_allocate_device();
	// ���� input_dev ����
	test_dev->name = "test_key";
	// �����¼����¼�ֵ
	// ���ò��������¼�
	__set_bit(EV_KEY, test_dev->evbit);
	//���ò�����Щ����ֵ����ʾ����豸Ҫ֧�� key_1
	__set_bit(KEY_1, test_dev->keybit);
	//�� Linux �ں�ע�� input_dev
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
	//3. �� beep_driver �ṹ��������� beep_probe �� beep_remove
	.probe = beep_probe,
	.remove = beep_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "class21_platform_",
		.of_match_table = of_match_table_test,
	},
	//4 .id_table �����ȼ�Ҫ�� driver.name �����ȼ�Ҫ�ߣ������� .id_table ����ƥ��
	.id_table = &beep_idtable
};
	
static int beep_driver_init(void)
{
	// 1.���ǿ������ļ�Ҫ�� init ������ʼ��
	int ret = 0;
	//2. �� init ��������ע���� platform_driver
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

