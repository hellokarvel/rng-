#include <intrins.h>
#include <reg52.h>
sbit LED = P1^7;
sbit LED_gear1=P2^1;
sbit LED_gear2=P2^2;
sbit LED_gear3=P2^3;														//LED用于显示风速大小
sbit dj=P1^0;														//电机端口					  												 
sbit DQ = P1^6;					  								//温度传感器数据段				
char table[11]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};	//共阴级，0-9	  		
int d2,d3,d4,d5,time,wen_du;										//4个变量+定时时间+温度变量
sbit key2=P1^2;														//时间加
sbit key3=P1^3;														//时间减
sbit key4=P1^4;														//时间开关														 
sbit w1=P2^4;	  													//数码管第一段，低电平有效
sbit w2=P2^5;														//数码管第二段，低电平有效
sbit w3=P2^6;														//数码管第三段，低电平有效	
sbit w4=P2^7;														//数码管第四段，低电平有效
int time=0;															
void DHT11_DATA();
void zi_keyscan();
void Timer1();
static unsigned char flag;
 ///////////////////////////////////////////////////////////////////////////////////////

void delay(unsigned int ms)																//延时函数，大约延时
{
	unsigned int x;
	while(ms--)
		for(x=10;x>0;x--); }
void delay_18B20(unsigned int i)
{
	while(i--);
}
/**********ds18b20初始化函数**********************/
void Init_DS18B20() 
{
	 unsigned char x=0;
	 DQ=1;          //DQ复位
	 delay_18B20(8);  //稍做延时
	 DQ=0;          //单片机将DQ拉低
	 delay_18B20(80); //精确延时 大于 480us
	 DQ=1;          //拉高总线
	 delay_18B20(14);
	 x=DQ;            //稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	 delay_18B20(20);
}
/***********ds18b20读一个字节**************/  
unsigned char ReadOneChar()
{
	unsigned char i=0;
	unsigned char dat=0;
	for (i=8;i>0;i--)
	 {
		  DQ=0; // 给脉冲信号
		  dat>>=1;
		  DQ=1; // 给脉冲信号
		  if(DQ)
		  dat|=0x80;
		  delay_18B20(4);
	 }
 	return(dat);
}
/*************ds18b20写一个字节****************/  
void WriteOneChar(unsigned char dat)
{
 	unsigned char i=0;
 	for (i=8;i>0;i--)
 	{
  		DQ=0;
 		DQ=dat&0x01;
    	delay_18B20(5);
 		DQ=1;
    	dat>>=1;
	}
}
/**************读取ds18b20当前温度************/
void ReadTemperature()
{
	char a=0;
	char b=0;
	char t=0;
	Init_DS18B20();
	WriteOneChar(0xCC);    	// 跳过读序号列号的操作
	WriteOneChar(0x44); 	// 启动温度转换
	delay_18B20(100);       // this message is wery important
	Init_DS18B20();
	WriteOneChar(0xCC); 	//跳过读序号列号的操作
	WriteOneChar(0xBE); 	//读取温度寄存器等（共可读9个寄存器） 前两个就是温度
	delay_18B20(100);
	a=ReadOneChar();    	//读取温度值低位
	b=ReadOneChar();   		//读取温度值高位
	wen_du=((b*256+a)>>4);    //当前采集温度值除16得实际温度值
}


																			   
void display()																	//数码管显示
{ if(wen_du==8)wen_du=24;
 if(wen_du==9)wen_du=25;
 if(wen_du==10)wen_du=26;
 if(wen_du==11)wen_du=27;
 if(wen_du==12)wen_du=28;
 if(wen_du==13)wen_du=29;
 if(wen_du==14)wen_du=30;
 if(wen_du==15)wen_du=31;
d2=wen_du/10;d3=wen_du%10;														//转换温度格式
 d4=time/10;d5=time%10;															//转换时间格式

	w1=0;P0=table[d2];delay(10); 												//第1位
	P0=0x00;w1=1;delay(1);		
								  
	w2=0;P0=table[d3];delay(10);												//第2位
	P0=0x00;w2=1;delay(1);

	w3=0;P0=table[d4]; delay(10);												//第3位
	P0=0x00;w3=1;delay(1);

	w4=0;P0=table[d5];delay(10);												//第4位
	P0=0x00;w4=1;delay(1);
							}



void zi_dong()																	//自动温控模式
{
	unsigned char i;
	zi_keyscan();																//按键扫描函数
																//调用显示函数
	if(wen_du<20||(time==0)){dj=0;}														//低于下限  档位为0 电机停止
	if((wen_du>=20)&&(wen_du<=25)&&(time!=0))												//温度大于下限，小于上限  1挡
	{ LED_gear1=1;LED_gear2=0;LED_gear3=0;
	for(i=0;i<3;i++){dj=0;display();zi_keyscan();}								//PWM通过延时得到
	 for(i=0;i<6;i++){dj=1;display();zi_keyscan();
	  }		
	}
	if((wen_du>25)&&(wen_du<=30)&&(time!=0))												//温度大于下限，小于上限  2挡
	{ LED_gear1=1;LED_gear2=1;LED_gear3=0;
	for(i=0;i<3;i++){dj=0;display();zi_keyscan(); }								//PWM通过延时得到
	 for(i=0;i<9;i++){dj=1;display();zi_keyscan();}		
	}
	if(wen_du>30&&(time!=0)){dj=1; LED_gear1=3;LED_gear1=1;LED_gear2=1;LED_gear3=1;}
	}																			//高温全速
void zi_keyscan()	                  					 						//按键定时关闭
{ 	 	
	display();
		 
	if(key2==0){LED=1;										 						//key2 定时增
	 delay(20);																    //防抖
	  if(key2==0)time=time+1;}
	while(!key2);		
					  	
    if(key3==0){										 						 //key3定时减
	 delay(20);
	  if(key3==0)time=time-1;}
 	while(!key3);

	if(key4==0){										                         //开始倒计时，单位秒
	 delay(20);
	  if(key4==0){ 
	     for(;time>0;time--) {
		 	display();delay(5800); }
	              }
	            }	
	while(!key4);
															    
	if(time==0){LED=0;
	  LED_gear1=0;
	  LED_gear2=0;
	  LED_gear3=0;};										                 //当时间为0时候，关闭电机
		 }

			
 void main()		  														//主函数
{	 
	unsigned char j;
	time=10;dj=0;		 														      //电机开
	  LED_gear1=0;
	  LED_gear2=0;
	  LED_gear3=0;
	
	while(1)			  													//进入while循环
	{ReadTemperature();															//读取温度值
	zi_dong();										//自动温控模式
		}
}													