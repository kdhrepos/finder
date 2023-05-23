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
	
	/* ==================== Control Start ==================== */ 
	while(1) 
	{
		message[0] = getch(); message[1] = '\0';
		if(message[0] =='p')
		{
			break;
		} 
		else if((message[0]=='w')||(message[0]=='a')||(message[0]=='s')||(message[0]=='d')||(message[0]=='q'))
		{
			write(sock, message, strlen(message));
		}
		else if((message[0]=='W')||(message[0]=='A')||(message[0]=='S')||(message[0]=='D')||(message[0]=='Q'))
		{
			write(sock, message, strlen(message));
		}
	}
	/* ==================== Control End ==================== */
	close(sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
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


