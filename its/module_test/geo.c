#include <linux/module.h>
#include <linux/kernel.h>

int geo_init(void)
{
		printk(KERN_INFO "Create a geo module!\n");
		return 0;
}

void geo_exit(void)
{
		printk(KERN_INFO "Exit geo module\n");
}


module_init(geo_init);
module_exit(geo_exit);
