/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-03-12 15:28
> Email        : 25915043@qq.com
> Filename     : simple.c
> Description  : simple module 
*******************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>

static int start,end;

int simple_init(void)
{
    printk(KERN_INFO "Loading Kernel Module\n");
    printk("The value of GOLDEN_RATIO_PRIME is:%llu\n", GOLDEN_RATIO_PRIME);
    
	start=jiffies;
	printk("The value of start jiffies is: %d\n",start);
    printk("The value of HZ is: %d\n",HZ );
      
	return 0;
}

void simple_exit(void)
{
    printk("The greatest common divisor of 3,300 and 24: %lu",gcd(3300,24));

    end=jiffies;
    printk("The value of end jiffies is: %d\n",end);
    printk(KERN_INFO "Removing Kernel Module\n");    
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("HaolinLiu");
