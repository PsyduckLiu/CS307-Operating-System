/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-04-01 21:13
> Email        : 25915043@qq.com
> Filename     : module_pid.c
> Description  : a pid module 
*******************************************************/

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>


#define BUFFER_SIZE 128
#define PROC_NAME "pid"

static long current_pid;
static ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
    .owner = THIS_MODULE,
    .read = proc_read,
    .write = proc_write,
};

static int proc_init(void) {
	printk(KERN_INFO "Loading Kernel Module\n");
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

static void proc_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
	printk(KERN_INFO "Removing Kernel Module\n"); 
}

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    struct task_struct *tsk = NULL;

    if (completed) {
        completed = 0;
        return 0;
    }

    tsk = pid_task(find_vpid(current_pid), PIDTYPE_PID);
    if(tsk) {
        rv = snprintf(buffer, BUFFER_SIZE,
                      "command = [%s], pid = [%ld], state = [%ld]\n",
                      tsk->comm, current_pid, tsk->state);
    } 
	else {
        printk(KERN_INFO "Invalid PID %ld!", current_pid);
        return 0;
    }

    completed = 1;

    if (raw_copy_to_user(usr_buf, buffer, rv)) {
        rv = -1;
    }
    return rv;
}

static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char *k_mem;

    k_mem = kmalloc(count, GFP_KERNEL);

    if (raw_copy_from_user(k_mem, usr_buf, count)) {
        printk( KERN_INFO "Error copying from user\n");
        return -1;
    }

    k_mem[count] = '\0';   
    kstrtol(k_mem, 10, &current_pid);
    printk(KERN_INFO "PID is %ld", current_pid);
    kfree(k_mem);

    return count;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Pid module");
MODULE_AUTHOR("HaolinLiu");


