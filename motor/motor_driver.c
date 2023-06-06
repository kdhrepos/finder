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

#define DEV_MAJOR_NUMBER 236 //디바이스 드라이버 주 번호
#define DEV_NAME "motor_driver" //디바이스 드라이버 이름
#define GPIO_SIZE 256 //GPIO 핀 맵 크기
#define GPIO_BASE 0xfe200000 //bcm2711 베이스 레지스터 주소

// 모터 드라이버 A Side 핀 번호
#define MOTOR_A_IN1_PIN 16
#define MOTOR_A_IN2_PIN 20

// 모터 드라이버 B Side 핀 번호
#define MOTOR_B_IN3_PIN 27
#define MOTOR_B_IN4_PIN 22

static void *mtr_map; //모터 드라이버 주소 할당 위한 변수
volatile unsigned *mtr;  //모터 핀 제어 변수

static int motor_open(struct inode * inode,struct file * file);
static int motor_write(struct file * file,const char * gdata,size_t length, loff_t * off);
static int motor_release(struct inode *minode, struct file *mfile);
static void set_low(void);

// 유저 레벨에서 제어하기 위한 유닉스 표준 함수 맵핑
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

    mtr_map = ioremap(GPIO_BASE, GPIO_SIZE); // mtr_map에 주소 할당
    if (!mtr_map) // 에러 처리
    {
        printk("error: mapping gpio memory\n");
        iounmap(mtr_map);
        return -EBUSY;
    }
    mtr = (volatile unsigned int *)mtr_map; // mtr_map 변수의 주소를 mtr 변수로 할당해 핀 제어

    /* Clear */
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
    // 0.05초 동안 움직임
    switch (ctrl[0])
    {
        case 'w':
        case 'W':{
            // Move Forward
            printk("Move Forward\n");

            // MOTOR_A_IN2_PIN : HIGH
            *(mtr + 7) = (0x01 << MOTOR_A_IN2_PIN);
            // MOTOR_B_IN4_PIN : HIGH
            *(mtr + 7) = (0x01 << MOTOR_B_IN4_PIN);
            // MOTOR_A_IN1_PIN : LOW
            *(mtr + 10) = (0x01 << MOTOR_A_IN1_PIN);
            // MOTOR_B_IN3_PIN : LOW
            *(mtr + 10) = (0x01 << MOTOR_B_IN3_PIN);

            mdelay(50);
            set_low(); // Stop
            break;
        }
        case 'a':
        case 'A':{
            // Move Left
            printk("Move Left\n");
            
            // MOTOR_A_IN2_PIN : HIGH
            *(mtr + 7) = (0x01 << MOTOR_A_IN2_PIN);
            // MOTOR_B_IN3_PIN : HIGH
            *(mtr + 7) = (0x01 << MOTOR_B_IN3_PIN);
            // MOTOR_A_IN1_PIN : LOW
            *(mtr + 10) = (0x01 << MOTOR_A_IN1_PIN);
            // MOTOR_B_IN4_PIN : LOW
            *(mtr + 10) = (0x01 << MOTOR_B_IN4_PIN);

            mdelay(50);
            set_low(); // Stop
            break;
        }
        case 's':
        case 'S':{
            // Move Backward
            printk("Move Backward\n");

            // MOTOR_A_IN1_PIN : HIGH
            *(mtr + 7) = (0x01 << MOTOR_A_IN1_PIN);
            // MOTOR_B_IN3_PIN : HIGH
            *(mtr + 7) = (0x01 << MOTOR_B_IN3_PIN);
            // MOTOR_A_IN2_PIN : LOW
            *(mtr + 10) = (0x01 << MOTOR_A_IN2_PIN);
            // MOTOR_B_IN4_PIN : LOW
            *(mtr + 10) = (0x01 << MOTOR_B_IN4_PIN);

            mdelay(50);
            set_low(); // Stop
            break;
        }
        case 'd':
        case 'D':{
            // Move Right
            printk("Move Right\n");
            // MOTOR_A_IN1_PIN : HIGH
            *(mtr + 7) = (0x01 << MOTOR_A_IN1_PIN);
            // MOTOR_B_IN4_PIN : HIGH
            *(mtr + 7) = (0x01 << MOTOR_B_IN4_PIN);
            // MOTOR_A_IN2_PIN : LOW
            *(mtr + 10) = (0x01 << MOTOR_A_IN2_PIN);
            // MOTOR_B_IN3_PIN : LOW
            *(mtr + 10) = (0x01 << MOTOR_B_IN3_PIN);
            
            mdelay(50);
            set_low(); // Stop
            break;
        }
        case 'q':
        case 'Q':{
            set_low(); // Stop
            break;
        }
        default:{
            break;
        }
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
    // GPCLR 레지스터에 모든 핀 Clear하여 모터 멈춰버림
    *(mtr + 10) = (0x01 << MOTOR_A_IN1_PIN);
    *(mtr + 10) = (0x01 << MOTOR_A_IN2_PIN);
    *(mtr + 10) = (0x01 << MOTOR_B_IN3_PIN);
    *(mtr + 10) = (0x01 << MOTOR_B_IN4_PIN);
}
module_init(motor_init);
module_exit(motor_exit);
MODULE_LICENSE("GPL");
