#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 3
#define MOTOR_FILE_NAME "/dev/motor_driver"

void error_handling(char *message);

int main(int argc, char *argv[])
{
        /* ==================== TCP/IP ========================================*/
        int serv_sock, clnt_sock;
        char message[BUF_SIZE];
        int str_len, i;

        struct sockaddr_in serv_adr;
        struct sockaddr_in clnt_adr;
        socklen_t clnt_adr_sz;

        if(argc!=3) {
                printf("Usage : %s <port>\n", argv[1]);
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
        int motor=open(MOTOR_FILE_NAME,O_RDWR);
        while(read(clnt_sock, message, BUF_SIZE)){
			if(message[0]=='p' || message[0]=='P'){
				break;
			}
        //      char * copied=(char *)malloc(sizeof(char)*BUF_SIZE);
        //      strcpy(copied,message);
             write(1,message,BUF_SIZE);
        //      write(motor,copied,BUF_SIZE);
                // write(motor,message,BUF_SIZE);
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