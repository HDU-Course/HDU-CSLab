#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>

static pid_t pid;
module_param(pid, int, 0644);

static int show_family_init(void)
{
	struct task_struct* p;
	struct task_struct* parent_p;
	struct task_struct* sibling_p;
	struct task_struct* children_p;
	
	struct list_head* sibling_head;
	struct list_head* children_head;
	
	p = pid_task(find_vpid(pid), PIDTYPE_PID);
	
	parent_p = p->parent;
	
	printk("me: %s %d %ld\n", p->comm, p->pid, p->__state);
	
	if(parent_p == NULL)
	{
		printk("No Parent!!!\n");
	}
	else
	{
		printk("Parent: %s %d %ld\n", parent_p->comm, parent_p->pid, parent_p->__state);
	}
	
	list_for_each(sibling_head, &parent_p->children)
	{
		sibling_p = list_entry(sibling_head, struct task_struct, sibling);
		if(sibling_p->pid != p->pid)
		{
			printk("Sibling: %s %d %ld\n", sibling_p->comm, sibling_p->pid, sibling_p->__state);
		}
	}
	
	list_for_each(children_head, &p->children)
	{
		children_p = list_entry(children_head, struct task_struct, sibling);
		printk("Children: %s %d %ld\n", children_p->comm, children_p->pid, children_p->__state);
	}
	
	return 0;
}
	
	
static void show_family_exit(void)
{
	printk(KERN_ALERT"goodbye!\n");
}

module_init(show_family_init);
module_exit(show_family_exit);

MODULE_LICENSE("GPL");



