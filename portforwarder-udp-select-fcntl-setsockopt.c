/* Archivo: portforwarder-udp-select-fcntl-setsockopt.c
 * 
 * Toma como argumentos del programa para l�nea
 * de comandos:
 * 		- PUERTO DE ENTRADA
 * 		- PUERTO DE SALIDA
 * 		- IP DE DESTINO
 * 		
 * Abre un socket UDP ('sockio', NO ORIENTADO A CONEXI�N).
 * 
 * Se utilizan dos estructuras sockaddr_in:
 * 
 * 	- 'sini' tiene como puerto al 'PUERTO DE ENTRADA' 
 *   	         y como direcci�n a 'INADDR_ANY' 
 *               (IP local de la propia computadora)
 *
 * 	- 'sino' tiene como puerto al 'PUERTO DE SALIDA'
 * 		 y como direcci�n a 'IP DE DESTINO'
 * 		 
 * Mediante fcntl(2) usa la operaci�n 'F_SETFL'
 * para asignar a la bandera del descriptor sockio el 
 * valor 'O_NONBLOCK' para funcionar en 
 * MODO NO BLOQUEANTE.
 * 
 * Mediante setsockopt(2) aplica al conector 'sockio'
 * la opci�n del nivel 'SOL_SOCKET' con el valor
 * 'SO_REUSEADDR' para evitar mensaje
 * 'Address already in use'
 * 
 * Verifica con select(2): 
 * 
 * Si existen datagramas de entrada 
 * 	los recibe por el conector 'sockio' y 
 * 	la direcci�n-puerto 'sini', 
 * entonces 
 * 	los env�a mediante el socket 'sockio' 
 * 	y la direcci�n-puerto 'sino' a la
 * 	'IP DE DESTINO'
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
#include<fcntl.h>

/* DEFINICIONES */
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
int main(int argc, char ** argv){
	if(argc < 4 || argc > 4){
		fprintf(stderr,	
			"Usa: %s  port_in  port_out  ipaddr_destiny \n",
			argv[0]);
		exit(-1);
	}
	const int yes = 1;
	int sockio, cuanto, largo, recibidos;

	struct sockaddr_in sini, sino;
	
	// ver: man 3 byteorder
	unsigned short int port_in, port_out;
	
	if(argv[1])
	{
		port_in = (unsigned short int)atoi(argv[1]);
		printf("\nport_in: %d \n", port_in);
	}
	else
		error("error: port_in");
	
	if(argv[2])
	{
		port_out = (unsigned short int)atoi(argv[2]);
		printf("port_out: %d \n", port_out);
	}
	else
		error("error: port_out");

	if(argv[3])
	{
		// Familia de direcciones para 'sino'
		sino.sin_family = AF_INET;
		// Puerto para 'sino'
		sino.sin_port = htons(port_out);
		// Direcci�n IP para 'sino'
		inet_pton(AF_INET, argv[3], &sino.sin_addr);
		/* POSIX TO NETWORK, ver man 3 inet_pton :
		 * transforma argv[3] 
		 * (la direccion IP pasada como argumento)
		 * desde formato ascii 
		 * (puntero a cadena de caracteres)
		 * al formato network, guarda en la 
		 * struct in_addr sino.sin_addr que a su
		 * vez es miembro de la 
		 * struct sockaddr_in sino . 
		 * Se trata de la direccion IP del host 
		 * al que quiere enviar datagramas,
		 * y pertenece a la familia de direcciones
		 * 'AF_INET' 
		 **/
	}
	else
		error("error: ipaddr_destiny bad address");
	
	//Buffer de 1024 bytes (caracteres)
	char linea[SIZE];
	
	/* Conjuntos de descriptores de ficheros 'in' 
	 * e 'in_orig' que supervisa select(2) */
	fd_set in, in_orig;

	/* Estructura que guarda el tiempo l�mite 
	 * hasta que retorne select(2) */
	struct timeval tv;
	
	//abre un conector UDP 'sockio'
	if((sockio = socket(PF_INET, SOCK_DGRAM, 0)) < 0 )
		error("socket UDP");

	/* Familia de direcciones de 'sockio'
	 * con direcci�n-puerto 'sini' */
	sini.sin_family = AF_INET;
	/* Puerto, con bytes en orden de red, 
	 * para 'sockio' con direcci�n-puerto 'sini' */
	sini.sin_port = htons(port_in);
	/* Direcci�n de internet, con bytes en orden de red,
	 * para 'sockio' */
	sini.sin_addr.s_addr = INADDR_ANY;
	
	// Asigna al conector 'sockio' la bandera 'O_NONBLOCK'
	if( fcntl(sockio, F_SETFL, O_NONBLOCK) < 0)
		error("fcntl");
	
	/* Mediante setsockopt(2) aplica al conector 'sockio'
	 * la opci�n del nivel 'SOL_SOCKET' con el valor 
	 * 'SO_REUSEADDR' para evitar mensaje 
	 * 'Address already in use' */
	if( setsockopt(sockio, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0)
		error("setsockopt");
	
	//publica direcci�n-puerto 'sini', del conector 'sockio'
	if( bind(sockio, (sad)&sini, sizeof sini) < 0 )
		error("bind");

	printf("\nESCUCHANDO EN DIRECCI�N LOCAL[ %s ] : PUERTO LOCAL[ %d ]\n",
			inet_ntoa(sini.sin_addr),
			ntohs(sini.sin_port));
	
	/* Tiene select(2) */
	// Limpia el conjunto de descriptores de ficheros 'in_orig'
	FD_ZERO(&in_orig);
	// A�ade 'sockio' al conjunto de descriptores 'in_orig'
	FD_SET(sockio, &in_orig);
		
	/* Tiene 1 hora */
	// Tiempo hasta que select(2) retorne: 3600 segundos
	tv.tv_sec = TIME;
	tv.tv_usec = 0;

	for(;;){
		// copia conjunto 'in_orig' en 'in'
		memcpy(&in, &in_orig, sizeof in);
		
		/* Espera a ver si se reciben datagramas por 'sockio'
		 * con direcci�n-puerto 'sini'*/
		if((cuanto = select( MAX(0,sockio)+1, &in, NULL, NULL, &tv) ) < 0 )
			error("select: error");
		// Si el tiempo de select(2) termina -> error timeout
		if(cuanto == 0)
			error("select: timeout");
		
		largo = sizeof sini;
		
		/* Si hay para leer desde el conector 'sockio' con
		 * direcci�n-puerto 'sini' */
		if(FD_ISSET(sockio, &in)){
			
			/* Recibe hasta 1024 caracteres de 'sockio'
			 * y los pone en el buffer 'linea' */
			if((recibidos = recvfrom(sockio, linea, SIZE, 0, (sad)&sini, &largo )) < 0)
				error("recvfrom");
			else if(recibidos == 0)	// Si recepci�n devuelve 0 -> parar ejecucion
				break;

			// Marca el fin del buffer con '0'
			// linea[recibidos]=0;
			
			/* Imprime en pantalla un aviso de que llega un datagrama 
			 * a la direcci�n-puerto local 'sini' */
			printf("\nA LA DIRECCI�N LOCAL[ %s ] : PUERTO LOCAL[ %d ] LLEGA DATAGRAMA\n",
					inet_ntoa(sini.sin_addr),
					ntohs(sini.sin_port));

			// Imprime el mensaje recibido
			// printf("%s \n",linea);
			
			/* Env�a contenido de 'linea' mediante 'sockio' con
			 * la direcci�n-puerto 'sino' a la 'IP DE DESTINO' */ 
			if(sendto(sockio, linea, sizeof linea, 0, (sad)&sino, largo) < 0 )
				error("sendto");
			
			/* Imprime en pantalla un aviso de que se env�a un datagrama 
			 * desde la direcci�n local 'sini.sin_addr'
			 * por el puerto local 'sino.sin_port' ('port_out')
			 * hacia la direcci�n remota 'sino.sin_addr' ('IP DE DESTINO')*/
			printf("\nDATAGRAMA ENVIADO DESDE LA DIRECCI�N LOCAL[ %s ] : PUERTO LOCAL[ %d ] A LA DIRECCI�N REMOTA [ %s ] \n",
					inet_ntoa(sini.sin_addr),
					ntohs(sino.sin_port),
					inet_ntoa(sino.sin_addr));
		}
	}
	close(sockio); // Cierra el conector UDP 'sockio'
	return 0; // Finalizaci�n exitosa
}
/* Fin Archivo: portforwarder-udp-select-fcntl-setsockopt.c */
