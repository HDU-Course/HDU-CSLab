#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>

//初始化函数
static int show_all_process_init(void)
{
	struct task_struct* p;
	printk(KERN_ALERT"          名称\t进程号\t状态   \t优先级\t父进程号\n");
	for_each_process(p)
	{
		if(p->mm == NULL)
		{
			printk(KERN_ALERT"%16s\t%-6d\t%-6ld\t%-6d\t%-6d\n", p->comm, p->pid, p->__state, p->normal_prio, p->parent->pid);
		}
		
	}
	return 0;
}

//清理函数
static void show_all_process_exit(void)
{
	printk(KERN_ALERT"goodbye!\n");
}

//函数注册
module_init(show_all_process_init);
module_exit(show_all_process_exit);

//模块许可声明
MODULE_LICENSE("GPL");
