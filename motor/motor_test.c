#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define MOTOR_FILE_NAME "/dev/motor_driver"// 모터 장치 파일 이름

int main(int argc, char **argv)
{
    int motor_fd;
    char data;
    motor_fd = open(MOTOR_FILE_NAME, O_RDWR); // 모터 장치 파일 Open
    if (motor_fd < 0) // Open 함수 오류 시 에러 처리
    {
        fprintf(stderr,"%s\n", strerror(errno));
        return -1;
    }
    // 모터 드라이버 Test
    while (1)
    {
        data = 'w'; // 앞으로 테스트
        write(motor_fd, &data, sizeof(char)); // 모터 드라이버에 신호 전달
    }
    close(motor_fd);
    return 0; 
}