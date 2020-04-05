/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-03-12 16:14
> Email        : 25915043@qq.com
> Filename     : seconds.c
> Description  : seconds module 
*******************************************************/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

static int start,end;

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
};

int proc_init(void)
{
	printk(KERN_INFO "Loading Kernel Module\n");
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    start = jiffies;
                
    return 0;
}

void proc_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
	printk(KERN_INFO "Removing Kernel Module\n"); 
}

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];    
    static int completed = 0;
    int time;

    end = jiffies;
    time = (end-start)/HZ;

    if(completed) {
        completed = 0;
        return 0;
    }
                    
    completed = 1;
            
    rv = sprintf(buffer, "The number of eplased seconds is %d.\n",time);
                            
    copy_to_user(usr_buf, buffer, rv);
                                
    return rv;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds module");
MODULE_AUTHOR("HaolinLiu");
