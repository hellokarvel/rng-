#include <stdio.h>

int Fc1(char *s,char *t){
	int i;
	for(i=0;s[i]==t[i];i++)
	 	if(s[i]=='\0')return 0;
	 	
	return s[i]-t[i];
	} 

int Fc2(char *s,char *t){

	for(;*s==*t;s++,t++)
	 	if(*s=='\0')return 0;
	 	
	return *s-*t;
	} 

int main(void){
	

	char s1[]="a,2,e,4,5";
	char t1[]="r,8,c,3,5";
	int Sum=Fc1(s1,t1);
     printf("%d\n",Sum);
	int Sum2=Fc2(s1,t1);
	 printf("%d",Sum2);
}