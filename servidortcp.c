#include<unistd.h>
#include <stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#define PORT 5000
  typedef struct sockaddr *sad;
  void error (char *s){
	  perror (s);
	  exit(-1);
  }
int main(){
	int s,s1;
	char linea [1024];
	int cto,L;
	struct sockaddr_in sin,sin1;
	if((s=socket(PF_INET,SOCK_STREAM,0))<0)error ("socket");
	sin.sin_family=AF_INET;
	sin.sin_port=htons(PORT);
	sin.sin_addr.s_addr=INADDR_ANY;
	if (bind(s,(sad) &sin,sizeof sin)<0)error("bind");
	if(listen (s,5)<0)error("listen");
         L=sizeof(sin1);
	if ((s1=accept(s,(sad)&sin1,&L))<0) 
		error("accept");
       	cto= read(s1,linea,1024);
	linea[cto]=0;
	printf("de %s:%d llega %s\n",inet_ntoa(sin1.sin_addr),ntohs(sin1.sin_port),linea);
	write(s1,"chau!",5);
	close(s1);close(s);
	return 0;
}
			
	





