#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>


#define DEV_MAJOR_NUMBER 237
#define DEV_NAME "led_driver"
#define GPIO_SIZE 256
#define GPIO_BASE 0xfe200000

#define LED_A_PIN 17
#define LED_B_PIN 23

static void *led_map;
volatile unsigned *led;
static int toggle=0;

static int led_open(struct inode * inode,struct file * file);
static int led_write(struct file * file,const char * gdata,size_t length, loff_t * off);
static int led_release(struct inode *minode, struct file *mfile);


static struct file_operations led_fops =
{
    .owner = THIS_MODULE,
    .open = led_open,
    .write = led_write,
    .release = led_release,
};
static int led_init(void)
{
    printk("Led Driver Init\n");
    /* register */
    register_chrdev(DEV_MAJOR_NUMBER, DEV_NAME, &led_fops);
    return 0;
}

static int led_open(struct inode * inode,struct file * file)
{
    printk("LED OPENED !!\n");
    led_map = ioremap(GPIO_BASE, GPIO_SIZE); 
    printk("led_map : %p",led_map);
    if (!led_map)
    {
        printk("error: mapping gpio memory");
        iounmap(led_map);
        return -EBUSY;
    }
    led = (volatile unsigned int *)led_map; 

    /* clear : 000 */
    *(led+(LED_A_PIN/10)) &= ~(0x7 <<(3*(LED_A_PIN%10)));
    *(led+(LED_B_PIN/10)) &= ~(0x7 <<(3*(LED_B_PIN%10)));

    /* set : output */
    *(led+(LED_A_PIN/10)) |= (0x1 << (3*(LED_A_PIN%10)));
    *(led+(LED_B_PIN/10)) |= (0x1 << (3*(LED_B_PIN%10)));

    return 0;
}

static int led_write(struct file * file,const char * ctrl,size_t length, loff_t * off)
{
    printk("LED WRITING!!\n");

    if(ctrl[0]=='t' || ctrl[0]=='T')
    {
        if(toggle==0)
        {
            toggle=1;
            *(led + 7) = (0x01 << LED_A_PIN);
            *(led + 7) = (0x01 << LED_B_PIN);
        }
        else
        {
            toggle=0;
            *(led + 10) = (0x01 << LED_A_PIN);
            *(led + 10) = (0x01 << LED_B_PIN);
        }
    }
 
    return length;
}

static void led_exit(void)
{
    printk("Led Driver Exit\n");
    /* unregister */
    unregister_chrdev(DEV_MAJOR_NUMBER, DEV_NAME);
}
static int led_release(struct inode *minode, struct file *mfile)
{
    if (led) iounmap(led);
    return 0;
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");