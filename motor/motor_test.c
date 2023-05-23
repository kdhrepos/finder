#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define MOTOR_FILE_NAME "/dev/motor_driver"// led device name

int main(int argc, char **argv)
{
    int motor_fd;
    char data;
    motor_fd = open(MOTOR_FILE_NAME, O_RDWR); // Device open
    if (motor_fd < 0) // If not open normally, close after error handling
    {
        fprintf(stderr,"%s\n", strerror(errno));
        return -1;
    }
    while (1)
    {
        data = 'w';
        write(motor_fd, &data, sizeof(char)); // Send data to the led driver
        // sleep(0.5);
    }
    close(motor_fd); // Close the device.
    return 0; // Terminate the program.
}