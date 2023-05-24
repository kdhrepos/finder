#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 2
#define MAX_SIZE 100000
#define MOTOR_DRIVER "/dev/motor_driver"
#define LED_DRIVER "/dev/led_driver"


void error_handling(char *message);
char reverse(char ctrl);
char comeback();


char backup_ctrl[MAX_SIZE];
int backup_len=0;

int motor,led;

int main(int argc, char *argv[])
{
        /* ==================== TCP/IP ========================================*/
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
                printf("Connected client\n");
        }
        /* ============================================================*/

        /* ==================== Finder Run ==================== */
        motor=open(MOTOR_DRIVER,O_RDWR);
        led=open(LED_DRIVER,O_RDWR);
        char message[BUF_SIZE];
        while(read(clnt_sock, message, BUF_SIZE))
        {       
                int message_len=strlen(message);
                
                /* save data for returning of the car */
                backup_ctrl[backup_len++]=reverse(message[0]);
                /* p :  Exit */
                if((message[0] =='p')||(message[0]=='P'))
		{
			break;
		} 
                /* r : return to base */
                else if((message[0]=='r')|| (message[0]=='R'))
                {
                        comeback();
                        backup_len=0;
                        continue;
                }
                /* send command to device driver */
		else if((message[0]=='w')||(message[0]=='a')||(message[0]=='s')||(message[0]=='d')||(message[0]=='q'))
		{
			write(motor,message,message_len);
		}
		else if((message[0]=='W')||(message[0]=='A')||(message[0]=='S')||(message[0]=='D')||(message[0]=='Q'))
		{
			write(motor,message,message_len);
		}
                /* led command to device driver*/
		else if((message[0] =='t')||(message[0]=='T'))
		{
			write(led, message,message_len);
		}
        }
        /* ==================== Finder End ==================== */

        close(motor);
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
char comeback()
{
        int i;
        backup_ctrl[backup_len]=0;

        char message[BUF_SIZE];
        for(i=backup_len-1; i>=0; i--)
        {
                message[0]=backup_ctrl[i]; message[1]='\0';
                write(motor,message,strlen(message));
                backup_ctrl[i]=0;
        }
}