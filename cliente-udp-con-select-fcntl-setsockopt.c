/* Archivo: cliente-udp-con-select-fcntl-setsockopt.c 
 * 
 * Abre un socket UDP (NO ORIENTADO A CONEXIÓN).
 * 
 * Mediante fcntl(2) usa la operación 'F_SETFL'
 * para asignar a la bandera del descriptor 'sockio'
 * el valor 'O_NONBLOCK'.
 *
 * Mediante setsockopt(2) aplica la opción del nivel
 * 'SOL_SOCKET' con el valor 'SO_REUSEADDR' al
 * conector 'sockio' 
 * 
 * Verifica con select(2) si existen DATAGRAMAS de
 * entrada por el socket ó por STDIN.
 * 
 * Si se reciben paquetes por STDIN, los 
 * envía por el socket (sendto(2)).
 *
 * Si se reciben paquetes por el socket, los 
 * envía a STDOUT (recvfrom(2)).
 **/

/* ARCHIVOS DE CABECERA */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<sys/select.h>
#include<string.h>
#include<fcntl.h>

/* DEFINICIONES */
#define PORT 5000
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define SIZE 1024
#define TIME 3600
				   
/* SINONIMOS */
typedef struct sockaddr *sad;

/* FUNCIONES */
void error(char *s){
    perror(s);
    exit(-1);
}

/* FUNCION PRINCIPAL MAIN */
int main(int argc, char **argv){
	if(argc < 2){
		fprintf(stderr,"usa: %s ipaddr \n", argv[0]);
		exit(-1);
	}
	const int yes = 1;
	int sockio, cuanto, largo, recibidos; 
	// Por sockio va a enviar-recibir paquetes datagramas
	
	// Direcciones-puertos de internet para 'sockio'
	struct sockaddr_in sini, sino;
	// 'sini' para recibir-leer
	// 'sino' para enviar-escribir
	
	// Buffer de 1024 caracteres (un arreglo)
	char linea[SIZE];
	// Conjuntos de descriptores de ficheros
	fd_set in, in_orig;
	// Tiempo limite hasta que select(2) retorne
	struct timeval tv;
	// abre conector UDP 'sockio'
	if((sockio = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
		error("socket");
	/* Familia de direcciones a la que pertenece 
	 * 'sino', para 'sockio' */
	sino.sin_family = AF_INET;
	// Puerto de 'sino', para 'sockio'
	sino.sin_port = htons(PORT);
    	inet_pton(AF_INET, argv[1], &sino.sin_addr);
	/* POSIX TO NETWORK, ver man 3 inet_pton : 
	 * transforma argv[1] 
	 * (la direccion IP pasada como argumento)
	 * desde formato ascii 
	 * (puntero a cadena de caracteres)
	 * al formato network, y guarda en la 
	 * 'struct in_addr sino.sin_addr' que a su vez
	 * es miembro de la 'struct sockaddr_in sino' .
	 * Se trata de la direccion IP del servidor
	 * al que quiere enviar datagramas, pertenece
	 * a la familia de direcciones 'AF_INET'
	 **/
	
	/* Asigna al conector 'sockio' mediante la 
	 * operación 'F_SETFL' el valor 'O_NONBLOCK' 
	 * con fcntl(2). */
	if( fcntl(sockio, F_SETFL, O_NONBLOCK) < 0)
		error("fcntl");	

	/* Mediante setsockopt(2) aplica la opción del nivel
	 * 'SOL_SOCKET' con el valor 'SO_REUSEADDR' al 
	 * conector 'sockio' */
	if(setsockopt(sockio, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0)
		error("setsockopt");
	
	/* Tiene select(2) */
	// Limpia el conjunto de descriptores de fichero 'in_orig'
	FD_ZERO(&in_orig);

	// Añade al conjuto 'in_orig' el descriptor STDIN
	FD_SET(0, &in_orig);

	// Añade al conjunto 'in_orig' el descriptor 'sockio'
	FD_SET(sockio, &in_orig);
	
	/* Tiene 1 hora */
	// Tiempo disponible hasta que select(2) regrese: 3600 segundos
	tv.tv_sec=TIME;
	tv.tv_usec=0;
	
	for(;;){
		// Copia contenido del conjunto 'in_orig' en 'in'
		memcpy(&in, &in_orig, sizeof in);
		
		// Ve si hay algún datagrama en el conjunto 'in'
		if((cuanto = select(MAX(0,sockio)+1, &in, NULL, NULL, &tv)) < 0)
			error("select: error");

		// Si el tiempo de espera de select(2) expira ==> timeout
		if(cuanto == 0)
			error("select: timeout");
		
		/* Averiguamos donde hay algo para leer */
		
		// Si hay para leer desde STDIN
		if(FD_ISSET(0,&in)){
			
			/* Lee hasta 1024 caracteres de STDIN y 
			 * pone en linea */
			fgets(linea, SIZE, stdin);
			
			// Envía contenido de linea en 'sockio'
			if(sendto(sockio, linea, sizeof linea, 0, (sad)&sino, sizeof sino) < 0)
				error("sendto");
		}
		
		largo = sizeof sini;
		
		// Si hay para leer desde 'sockio'
		if(FD_ISSET(sockio, &in)){
			
			/* Recibe hasta 1024 bytes desde 'sockio'
			 * mediante dirección-puerto 'sini' y
			 * guarda en linea */
			if((recibidos = recvfrom(sockio, linea, SIZE, 0, (sad)&sini, &largo)) < 0)
				error("recvfrom"); 
			else if(recibidos == 0)
				break; // Si recepción devuelve 0  ==> parar la ejecucion
			
			// Marcar el final del buffer con '0'
			linea[recibidos] = 0;
			
			/* Imprime en pantalla la dirección del servidor
			 * desde donde vienen datos */
			printf("\nDe la direccion[ %s ] : puerto [ %d ] --- llega el mensaje:\n",
					inet_ntoa(sini.sin_addr),
					ntohs(sini.sin_port));
			
			// Imprime en pantalla datos recibidos
			printf("%s \n",linea);
		}
	}
	close(sockio); // Cierra el conector UDP
	return 0; // Finalización exitosa
}
/* Fin Archivo: cliente-udp-con-select-fcntl-setsockopt.c */
