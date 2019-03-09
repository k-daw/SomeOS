#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

/* This function is called when the module is loaded. */ int simple init(void)
{       (KERN INFO "Loading Kernel Module∖n"); return 0;
}
/* This function is called when the module is removed. */ void simple exit(void)
{       (KERN INFO "Removing Kernel Module∖n"); }
/* Macros for registering module entry and exit points. */            (simple init);
           (simple exit);
MODULE LICENSE("GPL");
MODULE DESCRIPTION("Simple Module"); MODULE AUTHOR("SGG");
