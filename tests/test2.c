#include <AT898252.h>
long int a=0;
char *ptr=(char *)&a;
long int call(int X,char n){
	return X<<n;
}
void main(){
	P0=*(ptr++);
	a++;
	P1=*(ptr++);
	a++;
	P2=*(ptr++);
	a++;
	P3=*(ptr++);
	ptr=(char *)&a;
  a=call(a,*ptr);
}