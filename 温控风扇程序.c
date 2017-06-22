#include<reg51.h>
#include<intrins.h>			 //包含头文件
#define uchar unsigned char
#define uint unsigned int		 //宏定义
////////////////////
sbit dj=P1^0;//电机控制端接口
sbit DQ=P1^6;//温度传感器接口
//////////按键接口/////////////////////////////////
sbit key1=P3^5;//设置温度
sbit key2=P3^6;//温度加
sbit key3=P3^7;//温度减
//////////////////////////////////////////////////////
sbit w1=P2^4;
sbit w2=P2^5;
sbit w3=P2^6;
sbit w4=P2^7;			  //数码管的四个位
/////共阴数码管段选//////////////////////////////////////////////
uchar table[22]=
{0x3F,0x06,0x5B,0x4F,0x66,
0x6D,0x7D,0x07,0x7F,0x6F,
0x77,0x7C,0x39,0x5E,0x79,0x71,
0x40,0x38,0x76,0x00,0xff,0x37};//'-',L,H,灭,全亮，n	 16-21
uint wen_du;						//温度变量  
uint shang,xia; //对比温度暂存变量
uchar dang;//档位显示
uchar flag;
uchar d1,d2,d3;//显示数据暂存变量
void delay(uint ms)		//延时函数，大约延时25us
{
	uchar x;
	for(ms;ms>0;ms--)
		for(x=10;x>0;x--);
}
/***********ds18b20延迟子函数（晶振12MHz ）*******/ 
void delay_18B20(uint i)
{
	while(i--);
}
/**********ds18b20初始化函数**********************/
void Init_DS18B20() 
{
	 uchar x=0;
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
uchar ReadOneChar()
{
	uchar i=0;
	uchar dat=0;
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
void WriteOneChar(uchar dat)
{
 	uchar i=0;
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
	uchar a=0;
	uchar b=0;
	uchar t=0;
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
void display()//显示温度
{
	w1=0;P0=table[d1];delay(10); //第1位
	P0=0x00;w1=1;delay(1);
	 
	w2=0;P0=table[16];delay(10);//第2位
	P0=0x00;w2=1;delay(1);

	w3=0;P0=table[d2]; delay(10);//第3位
	P0=0x00;w3=1;delay(1);

	w4=0;P0=table[d3];delay(10);//第4位
	P0=0x00;w4=1;delay(1);
}
void zi_keyscan()//自动模式按键扫描函数
{
	if(key1==0)							  //设置键按下
	{
		delay(30);						  //延时去抖
		if(key1==0)flag=1;				  //再次判断按键，按下的话进入设置状态
		while(key1==0);//松手检测		  //按键释放
	}
	while(flag==1)						  //进入设置上限状态
	{
		d1=18;d2=shang/10;d3=shang%10;	  //显示字母H 和上限温度值
		display();						  //调用显示函数
		if(key1==0)						  //判断设置键是否按下
		{
			delay(30);					  //延时去抖
			if(key1==0)flag=2;			  //按键按下，进入设置下限模式
			while(key1==0);//松手检测
		}
		if(key2==0)						  //加键按下
		{
			delay(30);					  //延时去抖
			if(key2==0)					  //加键按下
			{
				shang+=5;				  //上限加5
				if(shang>=100)shang=100;  //上限最大加到100
			}while(key2==0);//松手检测
		}
		if(key3==0)						  //减键按下
		{
			delay(30);					  //延时去抖
			if(key3==0)					  //减键按下
			{
				shang-=1;				  //上限减1
				if(shang<=10)shang=10;	  //上限最小减到10
			}while(key3==0);//松手检测
		}		
	}
	while(flag==2)						  //设置下限
	{
		d1=17;d2=xia/10;d3=xia%10;		  //显示字母L 显示下限温度值
		display();						  //以下注释同上
		if(key1==0)
		{
			delay(30);
			if(key1==0)flag=0;
			while(key1==0);//松手检测
		}
		if(key2==0)
		{
			delay(30);
			if(key2==0)
			{
				xia+=5;
				if(xia>=95)xia=95;	
			}while(key2==0);//松手检测
		}
		if(key3==0)
		{
			delay(30);
			if(key3==0)
			{
				xia-=1;
				if(xia<=0)xia=0;	
			}while(key3==0);//松手检测
		}		
	}
}
void zi_dong()//自动温控模式
{
	uchar i;
	d1=dang;d2=wen_du/10;d3=wen_du%10;		//显示档位，显示当前温度值
	zi_keyscan();//按键扫描函数
	display();	 //调用显示函数
	if(wen_du<xia){dj=0;dang=0;}//低于下限  档位为0 电机停止
	if((wen_du>=xia)&&(wen_du<=shang))//温度大于下限，小于上限  1挡
	{
		dang=1;											//档位置1
		for(i=0;i<3;i++){dj=0;display();zi_keyscan();}	//PWM通过延时得到
		for(i=0;i<6;i++){dj=1;display();zi_keyscan();}		
	}
	if(wen_du>shang){dj=1;dang=2;}//高温全速
}
void main()		  //主函数
{
	uchar j;
	dj=0;		  //电机开
	shang=30;
	xia=20;		  //初始上下限值
	for(j=0;j<80;j++)	  //先读取温度值，防止开机显示85
	ReadTemperature();
	while(1)			  //进入while循环
	{	
		ReadTemperature();	//读取温度值
		for(j=0;j<100;j++)zi_dong();//自动温控模式
	}
}