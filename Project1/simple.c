/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-02-26 15:28
> Email        : 25915043@qq.com
> Filename     : simple.c
> Description  : a very simple module 
*******************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>

static int start,end;
static int time;
static char *name="HaolinLiu";

int simple_init(void)
{
    printk(KERN_INFO "Loading Kernel Module\n");
    printk("Hello, %s\n",name);
    printk("The The GOLDEN_RATIO_PRIME value is:%llu\n", GOLDEN_RATIO_PRIME);
    
    start=jiffies;
    
    printk("Current jiffies value is: %lu\n",jiffies);
    printk("The HZ value is: %d\n",HZ );
    
    return 0;
}

void simple_exit(void)
{
    printk("the gcd result is: %lu",gcd(3300,24));

    end=jiffies;
    time=(end-start)/HZ;

    printk("Current jiffies value is: %lu\n",jiffies);
    printk("%ds time has elapsed since the insertion.\n",time);
    printk(KERN_INFO "Removing Kernel Module\n");    
    printk("Goodbye, %s\n",name);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("Haolin Liu");
