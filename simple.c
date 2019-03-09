#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

/* This function is called when the module is loaded. */
int simple_init(void)
{       (KERN INFO "Loading Kernel Module∖n"); 
return 0;
}

void simple_exit(void)
{
    (KERN INFO "Removing Kernel Module∖n"); 
}

module_init(simple init);
module_exit(simple exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
