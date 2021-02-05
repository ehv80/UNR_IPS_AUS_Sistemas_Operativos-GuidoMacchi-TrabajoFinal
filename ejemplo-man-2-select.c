/* Archivo: ejemplo-man-2-select.c */
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(void)
{
	fd_set rfds;
	struct timeval tv;
	int valret;
	/* Mirar stdin (df 0) para ver si tiene entrada */
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	/* Esperar hasta 5 s */
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	valret = select(1, &rfds, NULL, NULL, &tv);
	/* ¡No confiar ahora en el valor de tv! */
	if (valret)
		printf("Los datos ya están disponibles.\n");
	/* FD_ISSET(0, &rfds) será verdadero */
	else
		printf("Ningún dato en 5 segundos.\n");
	return(0);
}
/* Archivo: ejemplo-man-2-select.c */
