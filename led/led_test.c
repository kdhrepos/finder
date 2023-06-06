#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define LED_FILE_NAME "/dev/led_driver"// led 장치 파일

int main(int argc, char **argv)
{
    int led_fd;
    char data;
    led_fd = open(LED_FILE_NAME, O_RDWR); // 유닉스 표준 함수 사용해 장치 파일 Open
    if (led_fd < 0) // 장치 파 일Open 시에 문제 있으면 오류 처리
    {
        fprintf(stderr,"Can't open %s\n", strerror(errno));
        return -1;
    }
    // led 2초간 껐다가 키며 Test
    while (1)
    {
        data = 't';
        write(led_fd, &data, sizeof(char)); // led 드라이버로 신호 write
        sleep(2); // 2초 대기
        data = 't';
        write(led_fd, &data, sizeof(char)); // led 드라이버로 신호 write
        sleep(2); // 2초 대기
    }
    close(led_fd);
    return 0;
}