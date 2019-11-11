#include<linux/module.h>
#include<linux/init.h>
#include<linux/major.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<asm/uaccess.h>

MODULE_LICENSE("GPL");

static int sk_major = 203, sk_minor = 0;
static int result;
static dev_t sk_dev;

static struct cdev sk_cdev;
struct file_operations sk_fops;
static int sk_register_cdev(void);

static int sk_open(struct inode *inode, struct file *filp);
static int sk_release(struct inode *inode, struct file *filp);
static int sk_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int sk_read(struct file *filp, const char buf, size_t count, loff_t *f_pos);
static int sk_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

static int sk_open(struct inode *inode, struct file *filp)
{
	printk("D: Device has been opened....\n");
	/* HW initailization */
	/* MOD_IND_USE_COUNT; */
	return 0;
}

static int sk_release(struct inode *inode, struct file *filp)
{
	printk("D: Device has been Closed....\n");
	return 0;
}

static int sk_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	char data[11];
	copy_from_user(data, buf, count);
	printk("D: data >>>>>>>  %s\n",data);

	return count;
}

static int sk_read(struct file *filp, const char buf, size_t count, loff_t *f_pos)
{
	char data[20] = "D: This is read\n";
	printk("D : copy data\n");
	copy_to_user(buf,data,count);

	return count;
}

static int sk_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd){
		case 'A':
		printk("K: cmd = A\n"); break;
		case 'B':
		printk("K: cmd = B\n"); break;
		case 'C':
		printk("K: cmd = C\n"); break;
		case 'D':
		printk("K: cmd = D\n"); break;
		case 'E':
		printk("K: cmd = E\n"); break;
		case 'F':
		printk("K: cmd = F\n"); break;
		case 'G':
		printk("K: cmd = G\n"); break;
		case 'H':
		printk("K: cmd = H\n"); break;
		default :
			return 0;
	}
	return 0;
}


struct file_operations sk_fops = {
	.open 	= sk_open,
	.release = sk_release,
	.write 	=sk_write,
	.read 	= sk_read,
	.unlocked_ioctl 	= sk_ioctl,
};


static __init int sk_init(void)
{
	printk("D: init SK Module is up...\n\n");

	if((result = sk_register_cdev())< 0){
		return result;
	}
	return 0;
}

static __exit void sk_exit(void)
{
	printk("D: exit The module is down...\n\n");
	cdev_del(&sk_cdev);
	unregister_chrdev_region(sk_dev, 1);
}

static int sk_register_cdev(void)
{
	int error;

	if(sk_major){
		sk_dev = MKDEV(sk_major, sk_minor);
		error = register_chrdev_region(sk_dev, 1, "sk");
	}
	else{
		error = alloc_chrdev_region(&sk_dev, sk_minor, 1, "sk");
		sk_major = MAJOR(sk_dev);
	}

	if(error<0){
		printk(KERN_WARNING "sk: Can't get major %d\n\n",sk_major);
		return error;
	}

	printk("Major Number = %d\n\n",sk_major);

	cdev_init(&sk_cdev, &sk_fops);
	sk_cdev.owner = THIS_MODULE;
	sk_cdev.ops = &sk_fops;
	error = cdev_add(&sk_cdev, sk_dev, 1);

	if(error)
		printk(KERN_NOTICE "sk Register Error %d\n\n",error);

	return 0;
}


module_init(sk_init);
module_exit(sk_exit);
