#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

// Module information
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple character device driver");
MODULE_VERSION("1.0");

// Device parameters
#define DEVICE_NAME "mychardev"
#define CLASS_NAME "mychar"
#define BUFFER_SIZE 1024
#define MAJOR_NUM 240

// IOCTL command definitions
#define RESET_BUFFER _IO('M', 1)
#define GET_BUFFER_SIZE _IOR('M', 2, int)
#define REVERSE_BUFFER _IO('M', 3)

// Global variables
static dev_t dev_num;
static struct cdev *mcdev;
static struct class *mclass;
static char *buffer;
static int buffer_pos = 0;
static DEFINE_MUTEX(buffer_mutex);

// Function prototypes
static int mychardev_open(struct inode *inode, struct file *file);
static int mychardev_release(struct inode *inode, struct file *file);
static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = mychardev_open,
    .release = mychardev_release,
    .read = mychardev_read,
    .write = mychardev_write,
    .unlocked_ioctl = mychardev_ioctl,
};

// Open function
static int mychardev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mychardev: Device opened\n");
    return 0;
}

// Release function
static int mychardev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "mychardev: Device closed\n");
    return 0;
}

// Read function
static ssize_t mychardev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    int error_count = 0;
    int bytes_read = 0;
    
    mutex_lock(&buffer_mutex);
    
    // Check if we have data to read
    if (buffer_pos == 0) {
        mutex_unlock(&buffer_mutex);
        return 0; // No data available
    }
    
    // Calculate how much to read
    if (count > buffer_pos)
        count = buffer_pos;
    
    // Copy data from kernel space to user space
    error_count = copy_to_user(buf, buffer, count);
    
    if (error_count == 0) {
        printk(KERN_INFO "mychardev: Read %zu bytes from device\n", count);
        bytes_read = count;
        buffer_pos = 0; // Reset buffer position after read
    } else {
        printk(KERN_INFO "mychardev: Failed to read %d bytes\n", error_count);
        bytes_read = -EFAULT;
    }
    
    mutex_unlock(&buffer_mutex);
    return bytes_read;
}

// Write function
static ssize_t mychardev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int error_count = 0;
    
    mutex_lock(&buffer_mutex);
    
    // Check if buffer has space
    if (count > BUFFER_SIZE) {
        printk(KERN_INFO "mychardev: Write too large (%zu bytes)\n", count);
        mutex_unlock(&buffer_mutex);
        return -EINVAL;
    }
    
    // Clear buffer before writing
    memset(buffer, 0, BUFFER_SIZE);
    
    // Copy data from user space to kernel space
    error_count = copy_from_user(buffer, buf, count);
    
    if (error_count == 0) {
        printk(KERN_INFO "mychardev: Wrote %zu bytes to device\n", count);
        buffer_pos = count;
    } else {
        printk(KERN_INFO "mychardev: Failed to write %d bytes\n", error_count);
        count = -EFAULT;
    }
    
    mutex_unlock(&buffer_mutex);
    return count;
}

// IOCTL function
static long mychardev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    
    mutex_lock(&buffer_mutex);
    
    switch (cmd) {
        case RESET_BUFFER:
            memset(buffer, 0, BUFFER_SIZE);
            buffer_pos = 0;
            printk(KERN_INFO "mychardev: Buffer reset\n");
            break;
            
        case GET_BUFFER_SIZE:
            ret = copy_to_user((int *)arg, &buffer_pos, sizeof(int));
            if (ret == 0) {
                printk(KERN_INFO "mychardev: Buffer size queried: %d\n", buffer_pos);
            } else {
                printk(KERN_INFO "mychardev: Failed to copy buffer size to user\n");
                ret = -EFAULT;
            }
            break;
            
        case REVERSE_BUFFER:
            if (buffer_pos > 0) {
                int i, j;
                char temp;
                for (i = 0, j = buffer_pos - 1; i < j; i++, j--) {
                    temp = buffer[i];
                    buffer[i] = buffer[j];
                    buffer[j] = temp;
                }
                printk(KERN_INFO "mychardev: Buffer reversed\n");
            }
            break;
            
        default:
            printk(KERN_INFO "mychardev: Unknown ioctl command: %u\n", cmd);
            ret = -EINVAL;
            break;
    }
    
    mutex_unlock(&buffer_mutex);
    return ret;
}

// Module initialization function
static int __init mychardev_init(void)
{
    int ret;
    
    printk(KERN_INFO "mychardev: Initializing the mychardev LKM\n");
    
    // Allocate buffer
    buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!buffer) {
        printk(KERN_ALERT "mychardev: Failed to allocate buffer\n");
        return -ENOMEM;
    }
    
    // Initialize buffer
    memset(buffer, 0, BUFFER_SIZE);
    buffer_pos = 0;
    
    // Allocate device number
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ALERT "mychardev: Failed to allocate device number\n");
        kfree(buffer);
        return ret;
    }
    
    // Create cdev structure
    mcdev = cdev_alloc();
    if (!mcdev) {
        printk(KERN_ALERT "mychardev: Failed to allocate cdev\n");
        unregister_chrdev_region(dev_num, 1);
        kfree(buffer);
        return -ENOMEM;
    }
    
    // Initialize cdev
    cdev_init(mcdev, &fops);
    mcdev->owner = THIS_MODULE;
    
    // Add cdev to system
    ret = cdev_add(mcdev, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ALERT "mychardev: Failed to add cdev\n");
        cdev_del(mcdev);
        unregister_chrdev_region(dev_num, 1);
        kfree(buffer);
        return ret;
    }
    
    // Create device class
    mclass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(mclass)) {
        printk(KERN_ALERT "mychardev: Failed to create device class\n");
        cdev_del(mcdev);
        unregister_chrdev_region(dev_num, 1);
        kfree(buffer);
        return PTR_ERR(mclass);
    }
    
    // Create device file
    if (device_create(mclass, NULL, dev_num, NULL, DEVICE_NAME) == NULL) {
        printk(KERN_ALERT "mychardev: Failed to create device file\n");
        class_destroy(mclass);
        cdev_del(mcdev);
        unregister_chrdev_region(dev_num, 1);
        kfree(buffer);
        return -1;
    }
    
    printk(KERN_INFO "mychardev: Device created successfully\n");
    printk(KERN_INFO "mychardev: Major number: %d, Minor number: %d\n", MAJOR(dev_num), MINOR(dev_num));
    
    return 0;
}

// Module cleanup function
static void __exit mychardev_exit(void)
{
    printk(KERN_INFO "mychardev: Cleaning up the mychardev LKM\n");
    
    // Remove device file
    device_destroy(mclass, dev_num);
    
    // Remove device class
    class_destroy(mclass);
    
    // Remove cdev
    cdev_del(mcdev);
    
    // Unregister device number
    unregister_chrdev_region(dev_num, 1);
    
    // Free buffer
    kfree(buffer);
    
    printk(KERN_INFO "mychardev: Goodbye!\n");
}

// Register init and exit functions
module_init(mychardev_init);
module_exit(mychardev_exit); 