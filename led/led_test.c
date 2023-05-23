#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define LED_FILE_NAME "/dev/led_driver"// led device name

int main(int argc, char **argv)
{
    int led_fd;
    char data;
    led_fd = open(LED_FILE_NAME, O_RDWR); // Device open
    if (led_fd < 0) // If not open normally, close after error handling
    {
        fprintf(stderr,"Can't open %s\n", strerror(errno));
        return -1;
    }
    while (1)
    {
        data = 1;
        write(led_fd, &data, sizeof(char)); // Send data to the led driver
        sleep(2);
        data = 0;
        write(led_fd, &data, sizeof(char));
        sleep(2);
    }
    close(led_fd); // Close the device.
    return 0; // Terminate the program.
}