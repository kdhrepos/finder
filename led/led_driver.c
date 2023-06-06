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

#define DEV_MAJOR_NUMBER 237 //디바이스 드라이버 주 번호
#define DEV_NAME "led_driver" //디바이스 드라이버 이름
#define GPIO_SIZE 256 //GPIO 핀 맵 크기
#define GPIO_BASE 0xfe200000 //bcm2711 베이스 레지스터 주소

//LED 드라이버에 쓸 핀 번호
#define LED_A_PIN 17
#define LED_B_PIN 23

static void *led_map; //led 드라이버 주소 할당 위한 변수
volatile unsigned *led; //led 핀 제어 변수
static int toggle=0; // led 드라이버 토글 형태로 만들기 위한 변수

static int led_open(struct inode * inode,struct file * file);
static int led_write(struct file * file,const char * gdata,size_t length, loff_t * off);
static int led_release(struct inode *minode, struct file *mfile);

// 유저 레벨에서 제어하기 위한 유닉스 표준 함수 맵핑
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
    led_map = ioremap(GPIO_BASE, GPIO_SIZE); // led_map에 주소 할당
    printk("led_map : %p",led_map);
    if (!led_map) // 에러 처리
    {
        printk("error: mapping gpio memory");
        iounmap(led_map);
        return -EBUSY;
    }
    led = (volatile unsigned int *)led_map; // led_map 변수의 주소를 led 변수로 할당해 핀 제어

    /* clear */
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
        if(toggle==0) // led off 일때
        {
            toggle=1; // led on
            //GPSET0 레지스터에 LED_A 핀, LED_B핀 두 개 001 할당 -> GPIO 핀 SET
            *(led + 7) = (0x01 << LED_A_PIN); 
            *(led + 7) = (0x01 << LED_B_PIN);
        }
        else // led on 일때
        {
            toggle=0; // led off
            //GPCLR0 레지스터에 LED_A 핀, LED_B핀 두 개 001 할당 -> 클리어
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