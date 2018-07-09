#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

#define CHAR_DEVICE_NAME  "customDev"

#define SUCCESS  0

#define MAX 50

static int inuse = 0;

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

static struct miscdevice char_dev_misc = {
  .minor = MISC_DYNAMIC_MINOR,
  .name  = CHAR_DEVICE_NAME,
  .fops  = &char_dev_fops
};

/* Register Driver with Misc Interface */
static int __init char_dev_init(void)
{
  int retval;

  /* Register a Miscellaneous Device */
  /* If MISC_DYNAMIC_MINOR is Specified then tries to allocate dynmically */
  /* Otherwise tries to assign specified Minor Number */
  /* Automatically creates an entry in /dev */
  retval = misc_register(&char_dev_misc);
  if(retval < 0)
  {  
    pr_err("Device Registration Failed with Minor Number %d\r\n",char_dev_misc.minor);
    return retval;
  }
  pr_info("Device Registered : %s with Minor Number : %d\r\n",CHAR_DEVICE_NAME, char_dev_misc.minor);
  return 0;
}

/* Unregister Driver */
static void __exit char_dev_exit(void)
{  
  /* Unregister the Miscellaneous Device */
  misc_deregister(&char_dev_misc);

  pr_info("Device Unregistered %s\r\n",CHAR_DEVICE_NAME);
}

/* Register the names of custom entry and exit routines */
module_init(char_dev_init);
module_exit(char_dev_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Character Driver Skeleton with Misc Interface");
MODULE_LICENSE("GPL");

