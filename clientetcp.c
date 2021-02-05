#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 5000
typedef struct sockaddr *sad;

void error(char*s)
{
	perror(s);
	exit(-1);
}

int main(int argc, char **argv)
{
	int sock;
	struct sockaddr_in sin;
	char linea [1024];
	int cto;
	if(argc<2){
		fprintf(stderr,"Usa:%s ipaddr\n", argv[0]);
		exit(-1);
	}
	if((sock=socket(PF_INET,SOCK_STREAM,0))<0)
		error("socket");
	sin.sin_family=AF_INET;
	sin.sin_port=htons(PORT);
	inet_aton(argv[1],&sin.sin_addr);
	if(connect(sock,(sad)&sin, sizeof sin) <0)
		error("connect");
	write(sock,"hola mundo!\n",12);
	cto=read (sock,linea,1024);
	linea[cto]=0;
	printf("contestaron%s\n", linea);
	close (sock);
	return 0;
}
