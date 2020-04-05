/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-03-12 15:48
> Email        : 25915043@qq.com
> Filename     : jiffies.c
> Description  : jiffies module 
*******************************************************/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "jiffies"

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,        
    .read = proc_read,
};

int proc_init(void)
{    
	printk(KERN_INFO "Loading Kernel Module\n");
    proc_create(PROC_NAME,0666,NULL,&proc_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME  );
    
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
    int rv=0;
    char buffer[BUFFER_SIZE];
    static int completed=0;
                
    if(completed){                    
        completed=0;                        
        return 0;                                
    }                
    
    completed=1;                    
    
    rv=sprintf(buffer,"The current value of jiffies is:%lu\n", jiffies);
    copy_to_user(usr_buf, buffer, rv);
                                
    return rv;                       
                                    
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Jiffies module");
MODULE_AUTHOR("HaolinLiu");
