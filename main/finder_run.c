#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 2 // 메시지 버퍼 사이즈
#define MAX_SIZE 100000 // return 명령 위한 배열 사이즈
#define MOTOR_DRIVER "/dev/motor_driver" // 모터 장치 파일 이름
#define LED_DRIVER "/dev/led_driver" // led 장치 파일 이름


void error_handling(char *message);
char reverse(char ctrl);
char comeback();


char backup_ctrl[MAX_SIZE]; // return 명령 위한 배열
int backup_len=0; // return 명령 위한 변수

int motor,led;

int main(int argc, char *argv[])
{
        /* ==================== TCP/IP 통신 (서버, 타겟 시스템) ========================================*/
        int serv_sock, clnt_sock;
        int str_len, i;

        struct sockaddr_in serv_adr;
        struct sockaddr_in clnt_adr;
        socklen_t clnt_adr_sz;

        if(argc!=3) {
                printf("Usage : <ip addr> <port>\n");
                exit(2);
        }

        serv_sock=socket(PF_INET, SOCK_STREAM, 0);   
        if(serv_sock==0)
                error_handling("socket() error");

        memset(&serv_adr, 0, sizeof(serv_adr));
        serv_adr.sin_family=AF_INET;
        serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
        serv_adr.sin_port=htons(atoi(argv[2]));

        if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
                error_handling("bind() error");

        if(listen(serv_sock, 5)==-1)
                error_handling("listen() error");

        clnt_adr_sz=sizeof(clnt_adr);

        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        if(clnt_sock==-1){
                error_handling("accept() error");
        }
        else{
                printf("Finder Run\n");
        }
        /* ============================================================*/

        /* ==================== Finder 시작 ==================== */
        motor=open(MOTOR_DRIVER,O_RDWR); // 모터 장치 파일 Open
        led=open(LED_DRIVER,O_RDWR); // led 장치 파일 Open
        char message[BUF_SIZE]; // 명령어 받아내기 위한 배열
        while(read(clnt_sock, message, BUF_SIZE))
        {       
                int message_len=strlen(message);

                /* return 기능 위해 명령어들 저장 */
                if(backup_len==MAX_SIZE-1){
                        backup_ctrl[0]='\0';
                        backup_len=0;
                }
                backup_ctrl[backup_len++]=reverse(message[0]);
                /* p : 종료 */
                if((message[0] =='p')||(message[0]=='P'))
		{
			break;
		} 
                /* r : 원래 자리로 복귀 */
                else if((message[0]=='r')|| (message[0]=='R'))
                {
                        comeback();
                        backup_len=0;
                        continue;
                }
                /* 모터 드라이버에 제어 명령 보냄 */
		else if((message[0]=='w')||(message[0]=='a')||(message[0]=='s')||(message[0]=='d')||(message[0]=='q'))
		{
			write(motor,message,message_len);
		}
		else if((message[0]=='W')||(message[0]=='A')||(message[0]=='S')||(message[0]=='D')||(message[0]=='Q'))
		{
			write(motor,message,message_len);
		}
                /* led 드라이버에 제어 명령 보냄 */
		else if((message[0] =='t')||(message[0]=='T'))
		{
			write(led, message,message_len);
		}
        }
        /* ==================== Finder 종료 ==================== */

        // 장치 파일, 소켓 파일 Close
        close(motor);
        close(led);
        close(clnt_sock);
        close(serv_sock);
        return 0;
}

void error_handling(char *message)
{
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}
// return 명령 위한 함수, w,a,s,d에 대한 명령어를 각각 반대로 반환
char reverse(char ctrl)
{
    switch (ctrl) {
        case 'w':
        case 'W':
            return 's';
        case 'a':
        case 'A':
            return 'd';
        case 's':
        case 'S':
            return 'w';
        case 'd':
        case 'D':
            return 'a';
        default:
            return ctrl;
    }
}
// return 명령 위한 주요 함수
char comeback()
{
        int i;
        backup_ctrl[backup_len]=0;

        char message[BUF_SIZE];
        // 배열의 명령어들을 거꾸로 읽어 원래 자리로 되돌아오게 만듦
        for(i=backup_len-1; i>=0; i--)
        {
                message[0]=backup_ctrl[i]; message[1]='\0'; // 모터 드라이버에 Write할 메시지 배열에 명령어 할당
                write(motor,message,strlen(message)); // 저장된 명령어 그대로 모터 드라이버에 Write
                backup_ctrl[i]=0; // NULL 삽입 -> 저장되었던 명령어 삭제
        }
}