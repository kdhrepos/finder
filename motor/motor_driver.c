#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/delay.h>

#define DEV_MAJOR_NUMBER 236
#define DEV_NAME "motor_driver"
#define GPIO_SIZE 256
#define GPIO_BASE 0xfe200000

#define MOTOR_A_IN1_PIN 27
#define MOTOR_A_IN2_PIN 22

#define MOTOR_B_IN3_PIN 16
#define MOTOR_B_IN4_PIN 20

static void *mtr_map;
volatile unsigned *mtr;

static int motor_open(struct inode * inode,struct file * file);
static int motor_write(struct file * file,const char * gdata,size_t length, loff_t * off);
static int motor_release(struct inode *minode, struct file *mfile);
static void set_low(void);


static struct file_operations motor_fops =
{
    .owner = THIS_MODULE,
    .open = motor_open,
    .write = motor_write,
    .release = motor_release,
};
static int motor_init(void)
{
    printk("Motor Driver Init\n");
    /* register */
    register_chrdev(DEV_MAJOR_NUMBER, DEV_NAME, &motor_fops);
    return 0;
}

static int motor_open(struct inode * inode,struct file * file)
{
    printk("Motor Driver Opened\n");

    mtr_map = ioremap(GPIO_BASE, GPIO_SIZE); // Physical addr. mapping
    if (!mtr_map) // mapping error handling
    {
        printk("error: mapping gpio memory\n");
        iounmap(mtr_map);
        return -EBUSY;
    }
    mtr = (volatile unsigned int *)mtr_map; 
    /* Clear for put 001*/
    // A Pins
    *(mtr+(MOTOR_A_IN1_PIN/10)) &= ~(0x07 <<(3*(MOTOR_A_IN1_PIN%10)));
    *(mtr+(MOTOR_A_IN2_PIN/10)) &= ~(0x07 <<(3*(MOTOR_A_IN2_PIN%10)));

    // B Pins
    *(mtr+(MOTOR_B_IN3_PIN/10)) &= ~(0x07 <<(3*(MOTOR_B_IN3_PIN%10)));
    *(mtr+(MOTOR_B_IN4_PIN/10)) &= ~(0x07 <<(3*(MOTOR_B_IN4_PIN%10)));

    /* Set */
    // A Pins
    *(mtr+(MOTOR_A_IN1_PIN/10)) |= (0x01 << (3*(MOTOR_A_IN1_PIN%10)));
    *(mtr+(MOTOR_A_IN2_PIN/10)) |= (0x01 << (3*(MOTOR_A_IN2_PIN%10)));

    // B Pins
    *(mtr+(MOTOR_B_IN3_PIN/10)) |= (0x01 << (3*(MOTOR_B_IN3_PIN%10)));
    *(mtr+(MOTOR_B_IN4_PIN/10)) |= (0x01 << (3*(MOTOR_B_IN4_PIN%10)));

    return 0;
}

static int motor_write(struct file * file,const char * ctrl,size_t length, loff_t * off)
{
    set_low();
    // Move for 0.1s
    switch (ctrl[0])
    {
        case 'w'|'W':{
            // Move Forward
            printk("Move Forward\n");

            // MOTOR_A_IN1_PIN : HIGH
            *(mtr + 7) |= (0x01 << MOTOR_A_IN1_PIN);
            // MOTOR_B_IN3_PIN : HIGH
            *(mtr + 7) |= (0x01 << MOTOR_B_IN3_PIN);
            // MOTOR_A_IN2_PIN : LOW
            *(mtr + 10) |= (0x01 << MOTOR_A_IN2_PIN);
            // MOTOR_B_IN3_PIN : LOW
            *(mtr + 10) |= (0x01 << MOTOR_B_IN4_PIN);

            mdelay(50);
            set_low();
            break;
        }
        case 'a'|'A':{
            // Move Left
            printk("Move Left\n");

            // MOTOR_A_IN2_PIN : HIGH
            *(mtr + 7) |= (0x01 << MOTOR_A_IN2_PIN);
            // MOTOR_B_IN3_PIN : HIGH
            *(mtr + 7) |= (0x01 << MOTOR_B_IN3_PIN);
            // MOTOR_A_IN1_PIN : LOW
            *(mtr + 10) |= (0x01 << MOTOR_A_IN1_PIN);
            // MOTOR_B_IN3_PIN : LOW
            *(mtr + 10) |= (0x01 << MOTOR_B_IN4_PIN);

            mdelay(50);
            set_low();
            break;
        }
        case 's'|'S':{
            // Move Backward
            printk("Move Backward\n");

            // MOTOR_A_IN2_PIN : HIGH
            *(mtr + 7) |= (0x01 << MOTOR_A_IN2_PIN);
            // MOTOR_B_IN3_PIN : HIGH
            *(mtr + 7) |= (0x01 << MOTOR_B_IN4_PIN);
            // MOTOR_A_IN1_PIN : LOW
            *(mtr + 10) |= (0x01 << MOTOR_A_IN1_PIN);
            // MOTOR_B_IN3_PIN : LOW
            *(mtr + 10) |= (0x01 << MOTOR_B_IN3_PIN);

            mdelay(50);
            set_low();
            break;
        }
        case 'd'|'D':{
            // Move Right
            printk("Move Right\n");
             // MOTOR_A_IN1_PIN : HIGH
            *(mtr + 7) |= (0x01 << MOTOR_A_IN1_PIN);
            // MOTOR_B_IN3_PIN : HIGH
            *(mtr + 7) |= (0x01 << MOTOR_B_IN4_PIN);
            // MOTOR_A_IN2_PIN : LOW
            *(mtr + 10) |= (0x01 << MOTOR_A_IN2_PIN);
            // MOTOR_B_IN3_PIN : LOW
            *(mtr + 10) |= (0x01 << MOTOR_B_IN3_PIN);

            mdelay(50);
            set_low();
            break;
        }
        case 'q'|'Q':{
            // Pause
            set_low();
            break;
        }
        default:{}
            break;
    }
    return length;
}
static int motor_release(struct inode *minode, struct file *mfile)
{
    if (mtr) iounmap(mtr);
    return 0;
}
static void motor_exit(void)
{
    printk("Motor Driver Exit\n");
    /* unregister */
    unregister_chrdev(DEV_MAJOR_NUMBER, DEV_NAME);
}
static void set_low(void)
{
    // Every Pin Low
    *(mtr + 10) |= (0x01 << MOTOR_A_IN1_PIN);
    *(mtr + 10) |= (0x01 << MOTOR_A_IN2_PIN);
    *(mtr + 10) |= (0x01 << MOTOR_B_IN3_PIN);
    *(mtr + 10) |= (0x01 << MOTOR_B_IN4_PIN);
}
module_init(motor_init);
module_exit(motor_exit);
MODULE_LICENSE("GPL");