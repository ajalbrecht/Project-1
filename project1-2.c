//

// Output: It has been 23 seconds since the project1-2 module was loaded
// It has been 30 seconds since the project1-2 module was loaded

//

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"
#define MESSAGE "It has been %lu seconds since the project1-2 module was loaded\n"

/**
 * Function prototypes
 */

unsigned long start_time = 0;

ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
        .owner = THIS_MODULE,
        .read = proc_read,
};

/* This function is called when the module is loaded. */
int proc_init(void) {
        // creates the /proc/hello entry
        // the following function call is a wrapper for
        // proc_create_data() passing NULL as the last argument
        proc_create(PROC_NAME, 0, NULL, &proc_ops);
        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

        start_time = jiffies;

	return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void) {
        // removes the /proc/hello entry
        remove_proc_entry(PROC_NAME, NULL);
        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time the /proc/hello is read,
 * and is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the 
 * corresponding /proc file.
 * params:
 * file:
 * buf: buffer in user space
 * count:
 * pos:
 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
        
        // computation time since the module was loaded
        unsigned long end_time = jiffies;  // get the curent value of jiffie when proc was called
        unsigned long time_inbetween = end_time - start_time;  // get the ammount of jiffies inbetween procInit and procRead
        unsigned int hertz_rate = (long)HZ;  // take the value of hz from the local machine and convert it to a long int
        unsigned long total = time_inbetween / hertz_rate; // calculate the ammount of seconds since the module was loaded
        
        int rv = 0;
        char buffer[BUFFER_SIZE];
        static int completed = 0;

        if (completed) {
                completed = 0;
                return 0;
        }

        completed = 1;

        // display the ammount of time it has been to the user
        rv = sprintf(buffer, "It has been %lu seconds since the project1-2 module was loaded\n", total);

        // copies the contents of buffer to userspace usr_buf
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}


/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Module");
MODULE_AUTHOR("SGG");
