#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <termio.h>

#define BUF_SIZE 2
void error_handling(char *message);
int getch(void);

int main(int argc, char *argv[])
{
	/* ==================== TCP/IP 통신 (클라이언트, 호스트 시스템) ====================*/
	int sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);   
	if(sock==-1)
		error_handling("socket() error");
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected...........");
	/* ============================================================*/

	/* ==================== 제어 시작 ==================== */ 
	while(1) 
	{
		message[0] = getch(); message[1] = '\0';
		/* p :  종료 */
		if((message[0] =='p')||(message[0]=='P'))
		{
			break;
		} 
		/* w,a,s,d : 모터 제어 명령 타겟 시스템에 write */
		else if((message[0]=='w')||(message[0]=='a')||(message[0]=='s')||(message[0]=='d')||(message[0]=='q'))
		{
			write(sock, message, 1);
		}
		else if((message[0]=='W')||(message[0]=='A')||(message[0]=='S')||(message[0]=='D')||(message[0]=='Q'))
		{
			write(sock, message, 1);
		}
		/* r : 복귀 명령 타겟 시스템에 write */
		else if((message[0]=='r')||(message[0]=='R'))
		{
			write(sock, message, 1);
		}
		/* t : led 제어 명령 타겟 시스템에 write */
		else if((message[0] =='t')||(message[0]=='T'))
		{
			write(sock, message, 1);
		}
	}
	/* ==================== 제어 종료 ==================== */
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
// 리눅스 환경에서 버퍼없이 문자 바로 받아내기 위한 함수
int getch(void)
{
    int ch;

    struct termios old;
    struct termios new;

    tcgetattr(0, &old);

    new = old;
    new.c_lflag &= ~(ICANON|ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;

    tcsetattr(0, TCSAFLUSH, &new);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &old);

    return ch;
}


