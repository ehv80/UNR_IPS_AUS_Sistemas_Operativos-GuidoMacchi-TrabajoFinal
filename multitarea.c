// Archivo: multitarea.c	Ejemplo de CORRUTINAS
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/* DEFINICIONES */
#define MAX_T 16
#define T_STACK (4*1024)
/* Para cambiar entre corrutinas */
#define TRANSFER(o,d) (setjmp(o)==0?(longjmp(d,1),0):0)

/* SINONIMOS */
typedef jmp_buf task_t;
typedef void (*pfunc)();

/* VARIABLES GLOBALES */
task_t t1, t2, tmain;

/* FUNCIONES */
void aux(unsigned int d, char * p, jmp_buf buf , void (*f)() )
{
	if(p-(char*)&d < d)aux(d,p,buf,f);
	else
	{
		if(setjmp(buf)!=0){
			f(); abort();
		}
	}
}
void hace_stack(task_t t, pfunc f)
{
	static int ctas = MAX_T;
	char dummy;
	aux (ctas*T_STACK,&dummy,t,f);
	ctas --;
}
void f1()
{
	double d;
	for(d=0;d<1000;d+=0.01){
		printf("en f1, d=%f\n",d);
		TRANSFER(t1,t2);
	}
}
void f2()
{
	int i;
	for( i=0 ; i<50 ; i++ )
	{
		printf("en f2, i=%d\n",i);
		TRANSFER(t2,t1);
	}
	TRANSFER(t2,tmain);
}

/* FUNCION PRINCIPAL MAIN */
int main()
{
	hace_stack(t1,f1);
	hace_stack(t2,f2);
	TRANSFER(tmain,t1);
	return 0;
}
// Fin Archivo: multitarea.c
