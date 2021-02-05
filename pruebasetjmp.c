#include <stdio.h>
#include <setjmp.h>

jmp_buf buf;

void f (int i)
{
	printf("en f\n");
	if(i!=0) longjmp(buf,1);
	printf("salimos de f\n");
}

int main()
{
	printf("uno\n");
	if(setjmp(buf)!=0){
		printf("!!!\n");
		return 0;
	}

	f(0); f(1);
	printf("chau\n");
	return 0;
}
