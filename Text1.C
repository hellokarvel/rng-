#include"reg52.h"
#include<intrins.h>	
sbit Dc=P1^0;//电机控制端接口
sbit key_onoff=P3^5;//时间总开关
sbit key_add=P3^6;//时间加
sbit key_decrease=P3^7;//时间减
int d8;
int time=0;


void zi_keyscan()	                   //定时器计数
{  	

if(key_add==0)							  //设置键按下
	{
	delay(30);						 
	if(key_add==0)time+=time;				  //再次判断按键，按下的话进入设置状态
	display();
	}
if(key_decrease==0)							  //设置键按下
	{
	delay(30);
	if(key_decrease==0)time-=time;
	display();				  //再次判断按键，按下的话进入设置状态
	
	}
 if(key_onoff==0){
 	delay(30);
	if(key_onoff==0==0){
        
	Timer1Init(time);
	for(){display();}
                               }
 
 }	
}
void Timer1Init(time)
{
	TMOD|=0X10;//选择为定时器1模式，工作方式1，仅用TR1打开启动。
    TH1=(65536-time*1000)/256;//每次定时time秒
    TL1=(65526-time*1000)%256;
        EA=1;  
 	ET1=1;//打开定时器1中断允许
	TR1=1;//打开定时器			
}
void time1() interrupt 1
 {	 dj=0;time=0;
 }
void main()
{	
	Timer1Init();  //定时器1初始化
		
}
 