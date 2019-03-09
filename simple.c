#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hash.h>

/* This function is called when the module is loaded. */
int simple_init(void)
{       
    printk(KERN_INFO "GOLDEN RATIO: %lu\n", GOLD_RATIO_PRIME); 
    printk(KERN_INFO "GDC: %lu\n", gcd(3300,24)); 
    return 0;
}

void simple_exit(void)
{
    printk(KERN_INFO "Removing Kernel Module∖n"); 
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
