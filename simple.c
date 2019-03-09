#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/param.h>
#include <linux/jiffies.h>

/* This function is called when the module is loaded. */
int simple_init(void)
{       
    printk(KERN_INFO "HZ :  %lu\n", HZ); 
    printk(KERN_INFO "jiffies: %lu\n", jiffies); 
    return 0;
}

void simple_exit(void)
{
    printk(KERN_INFO "Removing Kernel Module∖n"); 
    printk(KERN_INFO "jiffies: %lu\n", jiffies);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
