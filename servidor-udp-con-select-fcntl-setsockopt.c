/* Archivo: servidor-udp-con-select-fcntl-setsockopt.c
*  
*  Abre un socket UDP 'sockio' (NO ORIENTADO A CONEXIÓN). 
*  
*  Mediante fcntl(2) usa la operación 'F_SETFL' 
*  para asignar a la bandera del descriptor 'sockio'
*  el valor 'O_NONBLOCK'.
*
*  Mediante setsockopt(2) aplica la opción de nivel
*  'SOL_SOCKET' con el valor 'SO_REUSEADDR' al
*  conector 'sockio'. 
*  
*  Verifica con select(2) si existen datagramas de entrada
*  por el socket ó por STDIN.
*  
*  Si se reciben paquetes por el socket, los 
*  envía a STDOUT (recvfrom(2)).
*  
*  Si se reciben paquetes por STDIN, los 
*  envía por el socket (sendto(2)).
*/ 

/* ARCHIVOS DE CABECERA */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<sys/select.h>
#include <fcntl.h>

/* DEFINICIONES */
#define PORT 5000
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define SIZE 1024
#define TIME 3600

/* SINONIMOS*/
typedef struct sockaddr *sad;

/* FUNCIONES */
void error(char *s){
	perror(s);
	exit(-1);
}

/* FUNCION PRINCIPAL MAIN */
int main(){

	// Para setsockopt(2): SO_REUSEADDR
	const int yes = 1;
	int sockio, cuanto, largo, recibidos;

	// Dirección-puerto 'sinio' para 'sockio'
	struct sockaddr_in sinio;

	char linea[SIZE];
	fd_set in, in_orig;
	struct timeval tv;
	// Abre un conector UDP 'sockio'
	if((sockio = socket(PF_INET, SOCK_DGRAM, 0)) < 0 )
		error("socket");

	// Familia de direcciones de 'sinio', para 'sockio'
	sinio.sin_family = AF_INET;
	/* Puerto, con bytes en orden de red, de 'sinio' 
	 * para 'sockio' */
	sinio.sin_port = htons(PORT);
	/* Dirección de internet, con bytes en orden de red,
	 * de 'sinio', para 'sockio' */
	sinio.sin_addr.s_addr = INADDR_ANY; 

	// Asigna al conector 'sockio' la bandera 'O_NONBLOCK'
	if( fcntl(sockio, F_SETFL, O_NONBLOCK) < 0)
		error("fcntl");

	/* Mediante setsockopt(2) aplica la opción de nivel
	 * 'SOL_SOCKET' con el valor 'SO_REUSEADDR' al
	 * conector 'sockio' */
	if(setsockopt(sockio, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0)
		error("setsockopt");
	
	/* Publica la dirección-puerto 'sinio', 
	 * del conector 'sockio' */
	if( bind(sockio, (sad)&sinio, sizeof sinio) < 0 )
		error("bind");

	/* Tiene select(2) */
	/* Limpia el conjunto de descriptores 
	 * de ficheros 'in_orig' */
	FD_ZERO(&in_orig);
	// Añade STDIN al conjunto 'in_orig'
	FD_SET(0, &in_orig);
	//añade 'sockio' al conjunto 'in_orig'
	FD_SET(sockio, &in_orig);
		
	/* Tiene 1 hora */
	// Tiempo hasta que select(2) retorne: 3600 segundos
	tv.tv_sec=TIME;
	tv.tv_usec=0;

	for(;;){
		// Copia conjunto 'in_orig' en 'in'
		memcpy(&in, &in_orig, sizeof in);
		
		/* Espera a ver si se reciben datagramas por STDIN
		 * o por 'sockio' */
		if( (cuanto = select( MAX(0,sockio)+1, &in, NULL, NULL, &tv) ) < 0 )
			error("select: error");
		// Si el tiempo de select(2) termina -> error timeout
		if(cuanto == 0)
			error("select: timeout");
				
			largo = sizeof sinio;
			
		/* Averigua donde hay algo para leer*/
		
		// Si hay para leer desde el conector 'sockio'
		if(FD_ISSET(sockio, &in)){
			
			/* Recibe hasta 1024 caracteres de 'sockio'
			 * y los pone en 'linea' */
			if((recibidos = recvfrom(sockio, linea, SIZE, 0, (sad)&sinio, &largo )) < 0)
				error("recvfrom");
			else if(recibidos == 0)	// Si recepción devuelve 0 
				break;		// parar la ejecución

			// Marca el fin del buffer con '0'
			linea[recibidos]=0;
			
			/* Imprime en pantalla dirección de internet 
			 * del cliente desde donde vienen datos */
			printf("\nDe la direccion[ %s ] : puerto[ %d ] --- llega el mensaje:\n",
					inet_ntoa(sinio.sin_addr),
					ntohs(sinio.sin_port));

			// Imprime el mensaje recibido
			printf("%s \n",linea);
		}
		
		// Si hay para leer desde STDIN
		if(FD_ISSET(0,&in)){
			
			/* Lee hasta 1024 caracteres de STDIN, 
			 * los pone en 'linea' */
			fgets(linea, SIZE, stdin);
			
			// Envía contenido de 'linea' en 'sockio'
			if(sendto(sockio, linea, sizeof linea, 0, (sad)&sinio, largo) < 0 )
				error("sendto");
		}
	}
	close(sockio); 	// Cierra el conector UDP 'sockio'
	return 0;	// Finalización exitosa
}
/* Fin Archivo: servidor-udp-con-select-fcntl-setsockopt.c */
