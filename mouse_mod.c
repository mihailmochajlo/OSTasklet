#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

MODULE_LICENSE( "GPL" );

cycles_t cycles1, cycles2;
static unsigned long count = 0;

void pressed(unsigned long data)
{
	count++;
	printk(KERN_INFO "[mouse_mod] Mouse buttons was pressed/released %ld times\n", count);
	cycles2 = get_cycles();
	printk("\ncycles2 [%lld]\n", (long long unsigned) cycles2);
}

DECLARE_TASKLET(mouse_tasklet, pressed, (unsigned long)&count);

irqreturn_t mouse_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	if (irq == 12)
	{
		cycles1 = get_cycles();
		printk("\ncycles1 [%lld]\n", (long long unsigned) cycles1);
		tasklet_schedule(&mouse_tasklet);
		return IRQ_HANDLED;
	}
	else
		return IRQ_NONE;
}

int init_module () 
{
	int result = 0;
	
	int req_result = request_irq(12, (irq_handler_t) mouse_handler,
				 IRQF_SHARED, "mouse_irq_handler",
				 (void *)(mouse_handler));

	if (req_result != 0)
	{
		printk(KERN_ALERT "[mouse_mod] Request IRQ error!\n");
		result = -1;
	}
	
	printk(KERN_INFO "[mouse_mod] Request IRQ success!\n");
	printk(KERN_INFO "[mouse_mod] Module is now loaded.\n");
	
	return result;
}

void cleanup_module () 
{
	free_irq(12, (void *) mouse_handler);
	tasklet_kill(&mouse_tasklet);
	printk(KERN_INFO "[mouse_mod] Module is now unloaded.\n");
}

//module_init(module_init);
//module_exit(module_cleanup);

