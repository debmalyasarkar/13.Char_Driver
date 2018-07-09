#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define CHAR_DEVICE_NAME  "customDev"

#define SUCCESS  0

#define MAX 50

static dev_t device_id;
static struct class *device_class;
static int inuse = 0;

unsigned int major = 0;
unsigned int minor = 0;

char rdbuffer[MAX];
char wrbuffer[MAX] = "Manners Maketh A Man";

static int char_dev_open(struct inode *inode, struct file *file)
{
  if(inuse)
  {
    pr_err("Device Busy %s\r\n",CHAR_DEVICE_NAME);
    return -EBUSY;
  }
  inuse = 1;
  pr_info("Open Operation Invoked\r\n");
  return SUCCESS;
}

static int char_dev_release(struct inode *inode, struct file *file)
{
  inuse = 0;
  pr_info("Release Operation Invoked\r\n");
  return SUCCESS;
}

static ssize_t char_dev_read(struct file *file, char __user *buf, size_t size, loff_t *off)
{
  copy_to_user(buf,wrbuffer,size);
  return 0;
}

static ssize_t char_dev_write(struct file *file, const char __user *buf, size_t size, loff_t *off)
{
  copy_from_user(rdbuffer,buf,size);
  pr_info("Received : %s , Len = %u\r\n",rdbuffer,size);
  return SUCCESS;
}

static struct file_operations char_dev_fops = {
  .owner   = THIS_MODULE,
  .open    = char_dev_open,
  .release = char_dev_release,
  .read    = char_dev_read,
  .write   = char_dev_write
};

/* Register Driver with I/O Abstraction Layer */
static int __init char_dev_init(void)
{
  int retval;

  /* Acquire Major Number by using Helper Functions */
  /* If Major is 0 attempts to acquire dynamically */
  /* If Major is Positive then attempts to allocate that Major Number */
  retval = register_chrdev(major, CHAR_DEVICE_NAME, &char_dev_fops);
  if(retval < 0)
  {  
    pr_err("Error Registering Device Driver\r\n");
    return retval;
  }

  /* Print the acquired Major Number */
  major = retval;
  pr_info("Major Number is %d\r\n",major);

  /* Create Device ID from Major and Minor Numbers */
  device_id  = MKDEV(major, minor);

  /* Automatically create an entry in /sysfs */
  device_class = class_create(THIS_MODULE, "VIRTUAL");

  /* Automatically create device node */
  device_create(device_class, NULL, device_id, NULL, CHAR_DEVICE_NAME);

  pr_info("Device Registered %s\r\n",CHAR_DEVICE_NAME); 
  return 0;
}

/* Unregister Driver */
static void __exit char_dev_exit(void)
{  
  device_destroy(device_class, device_id);

  class_destroy(device_class);
 
  /* Delete cdev Object and free the allocated major number */
  unregister_chrdev(major, CHAR_DEVICE_NAME);

  pr_info("Device Unregistered %s\r\n",CHAR_DEVICE_NAME);
}

/* Register the names of custom entry and exit routines */
module_init(char_dev_init);
module_exit(char_dev_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Character Driver Skeleton with Registration Helper Functions");
MODULE_LICENSE("GPL");

