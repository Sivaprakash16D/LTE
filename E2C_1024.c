#include <p30f6012A.h>
#include <string.h>
#include "E2C_1024.h"
#include "LCD_DISP.h"
#include "Timer.h"

#define EWP_A_Latch  _LATD8
#define EWP_B_Latch  _LATD9
#define ESCL_Latch   _RG2
	
#define ESDA_Direction  _TRISG3
#define ESDA_Port  _RG3
#define ESDA_Latch _LATG3

#define Input_Direction 	1
#define Output_Direction	0
#define Output_Enable       1
#define Output_Disable 		0
void Nop()
{}
void Write_E2(unsigned long dest,char* source,int x)//write to serail E2prom
{
	int i;
	unsigned int j;
	unsigned char destlow,desthigh;
//    char TBuff[16] = {0};
		
	EWP_B_Latch = Output_Disable;Nop();
	EWP_A_Latch = Output_Disable;Nop();
	
	E2Delay();

	destlow = dest;
	desthigh = dest >> 8;

	for(i=0;i<x;i++)
	{
		
		StartCmd();	

		E2Delay();
		E2Delay();	
		
		if( dest > 0x2FFFF)   // sending 'Write operation enable command'
			Send_E2(0xaa);
		else if(dest > 0x1FFFF)
			Send_E2(0xa2);		
		else if(dest > 0xFFFF )
			Send_E2(0xa8);						
		else			
			Send_E2(0xa0);
		
		destlow = dest & 0xff;
		desthigh = (dest & 0xff00) >> 8;
		dest++;

			E2Delay();	
			
		SerAck();
		Send_E2(desthigh);		// Sending Higher order address

		SerAck();
		Send_E2(destlow);	// Sending Lower order address

/*		Send_E2((destlow) + i);	// Sending Lower order address
		
		if((destlow + i) == 0xff)
			desthigh = desthigh + 1;
*/
       
		SerAck();	
		Send_E2(*(source+i));	// Sending 'Data to be stored'
		SerAck();
		StopCmd();

		E2Delay();

		for(j=0;j<4000;j++);
	
	}
//	E2Delay();
	EWP_A_Latch = Output_Enable;Nop();
	EWP_B_Latch = Output_Enable;Nop();
	return;
}

void Read_E2(char* dest,unsigned long source,int x)
{
	unsigned int  ei;
//	unsigned int j;
	unsigned char sourcelow,sourcehigh;

	sourcelow = source;
	sourcehigh = source >> 8;

	for(ei=0;ei<x;ei++)
	{
		StartCmd();		
		E2Delay();
		E2Delay();


		E2Delay();
		E2Delay();
		
		if( source > 0x2FFFF)   // sending 'Write operation enable command'
			Send_E2(0xaa);
		else if(source > 0x1FFFF)
			Send_E2(0xa2);		
		else if(source > 0xFFFF )
			Send_E2(0xa8);						
		else 
			Send_E2(0xa0);

		sourcelow = source & 0xff;
		sourcehigh = (source & 0xff00) >> 8;


			E2Delay();

		SerAck();
		Send_E2(sourcehigh);		// Sending Higher order address
		SerAck();
//		Send_E2((sourcelow + ei));	// Sending Lower order address
		Send_E2(sourcelow);	// Sending Lower order address
		SerAck();

//		if((sourcelow + ei) == 0xff)
//			sourcehigh = sourcehigh + 1;

		E2Delay();
		StartCmd();
		E2Delay();


 		E2Delay();
		E2Delay();
		E2Delay();
			
		
		if(source > 0x2FFFF)	// sending 'Write operation enable command'
			Send_E2(0xab);
		else if(source > 0x1FFFF)
			Send_E2(0xa3);
		else if(source > 0xFFFF)
			Send_E2(0xa9);						
		else 					
			Send_E2(0xa1);	
		
		source++;	

				E2Delay();

		SerAck();	
		*(dest+ei) = Get_E2();		// Reading the actual data from E2		
		
		E2Delay();
		SerNoAck();
		E2Delay();

		StopCmd();
		E2Delay();
		E2Delay();		
	}
	return;
}

void Send_E2(char c)
{
	char i;
	unsigned char j = 128;
	for(i=7;i>=0;i--)
	{
		if((c & j) == j)
		{
			ESDA_Latch = 1;;//Delay(1);
		}
		else	
		{
			ESDA_Latch = 0;//Delay(1);
		}
		SerClock();		
		j = j/2;
		Nop();
			Nop();Nop();Nop();Nop();		
	}

	return;
}

void SerClock()
{
	ESCL_Latch = 1;Nop();//Delay(1);
	E2SDelay();
	ESCL_Latch = 0;Nop();//Delay(1);

	return;
}

void SerAck()
{
//	ESDA_Latch = 1;Nop();//Delay(1);
	ESDA_Latch = 0;
	SerClock();

	return;
}
void SerNoAck()
{
	ESDA_Latch = 1;
//	ESDA_Latch = 1;Nop();//Delay(1);
	SerClock();

	return;
}
void StartCmd()
{
	ESDA_Latch = 1;Nop();//Delay(1);
	ESCL_Latch = 1;Nop();//Delay(1);
	ESDA_Latch = 0;Nop();//Delay(1);
	E2SDelay();
	ESCL_Latch = 0;Nop();//Delay(1);

	return;
}

void StopCmd()
{
	ESDA_Latch = 0;Nop();//Delay(1);
	ESCL_Latch = 1;Nop();//Delay(1);
	E2SDelay();
	ESDA_Latch = 1;Nop();//Delay(1);

	return;
}

unsigned char Get_E2()
{
	char i = 0;
	unsigned char val = 0;
	unsigned char j = 128;
	ESDA_Direction = Input_Direction;Nop();
			Nop();Nop();Nop();Nop();
	//Delay(1000);
	for(i=8;i>0;i--)
	{
		//if(ESDA_Latch)
		if(ESDA_Port)
		val = val+j;
		j = j/2;
		SerClock();
		Nop();Nop();Nop();		
	}
	ESDA_Direction = Output_Direction;Nop();
			Nop();Nop();Nop();Nop();
		//Delay(1000);
	return(val);
}

void E2Delay()
{
	char i;
	for(i=0;i<100;i++);
	return;
}
void E2SDelay()
{
	char i;
	for(i=0;i<100;i++);
	return;
}

void WriteToCode(char* Destination,char* Source,unsigned int TotalBytes)
{
	unsigned char i;

	for(i=0;i<TotalBytes;i++)
		*(Destination+i)=*(Source+i);
}
