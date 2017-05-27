#include<stdio.h>
#define P1(a,b)      printf("a#cc",#a,a);  printf("%d:%d\n",a,a);
 #define  P2(a, b)     do{;printf("%d\n",a);printf("%d\n",b);}while(0) 
  #define   Cat(a,b)    a##b   
int main(){
	

int n = Cat(123,15);

printf("%d",n);

}