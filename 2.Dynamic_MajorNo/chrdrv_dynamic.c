#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define CHAR_DEVICE_NAME  "customDev"

#define SUCCESS  0

#define MAX 50

static dev_t device_id;
static struct cdev *device_cdev;
static int count = 1, inuse = 0;

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

/* Register Driver with I/O Abstraction Layer
 *
 * Step 1 : Acquire Major Number Dynamically
 * Step 2 : Register File Operations with devtmpfs
 *
*/
static int __init char_dev_init(void)
{
  int retval,startminor = 0;

  /* Register a range of device numbers dynamically */
  alloc_chrdev_region(&device_id, startminor, count, CHAR_DEVICE_NAME);

  /* Allocate cdev instance */
  device_cdev = cdev_alloc();

  /* Initialize cdev with fops object */
  cdev_init(device_cdev, &char_dev_fops);

  /* Register cdev with vfs(devtmpfs) */
  retval = cdev_add(device_cdev, device_id, count);
  if(retval < 0)
  {  
    pr_err("Error Registering Device Driver\r\n");
    return retval;
  }

  /* Print the Allocated Major and Minor Numbers */
  pr_info("Major Number = %d, Minor Number = %d\r\n", MAJOR(device_id),MINOR(device_id));

  pr_info("Device Registered %s\r\n",CHAR_DEVICE_NAME); 
  return 0;
}

/* Unregister Driver */
static void __exit char_dev_exit(void)
{
  /* Remove cdev object */
  cdev_del(device_cdev);
 
  /* Free the major and minor numbers used */
  unregister_chrdev_region(device_id,count);
  pr_info("Device Unregistered %s\r\n",CHAR_DEVICE_NAME);
}

/* Register the names of custom entry and exit routines */
module_init(char_dev_init);
module_exit(char_dev_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Character Driver Skeleton with Dynamic Major Number");
MODULE_LICENSE("GPL");

