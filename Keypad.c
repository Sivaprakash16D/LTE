#include <p30f6012A.h>
#include <string.h>
#include <stdlib.h>
/*
#define	KEY1				_RB12
#define KEY2				_RB13
#define	KEY3				_RB14
#define KEY4				_RB15
*/

#define ON 1
#define OFF 0

#define LOW 0
#define HIGH 1
/*
#define	PORT1_2_INPUTROW 				_RB2
#define PORT1_3_INPUTROW 				_RB3
#define	PORT1_4_OUTPUTCOL				_RB4
#define PORT1_5_OUTPUTCOL				_RB5	//Not used
*/

#define	KEY1_Direction		_TRISB2
#define	KEY2_Direction		_TRISB3
#define	KEY3_Direction		_TRISB4
#define	KEY4_Direction		_TRISB5


unsigned char MenuKey = 0;
unsigned char EnterKey = 0;
unsigned char TareKey = 0;
unsigned char UpKey = 0;
unsigned char LeftKey = 0;		
unsigned char EscKey = 0;
unsigned char KeyRead = 1;
unsigned char KeyReadClear = 1;

void KeySense(void);

/*  Keypad 

void KeySense()
{
	static char Cnt; 
	Cnt=0;
	while(1)
	{
		EnterKey = MenuKey = LeftKey = UpKey = EscKey = TareKey = 0;
		PORT1_4_OUTPUTCOL = OFF;Nop();Nop();Nop();
		PORT1_5_OUTPUTCOL = ON;Nop();Nop();Nop();					
		if(PORT1_2_INPUTROW == LOW || PORT1_3_INPUTROW == LOW)
		{
			if(PORT1_2_INPUTROW == HIGH && PORT1_3_INPUTROW == LOW && KeyRead ==1)
			 {					
				PORT1_4_OUTPUTCOL = OFF;Nop();Nop();Nop();
				PORT1_5_OUTPUTCOL = OFF;Nop();Nop();Nop();
				UpKey=1;					
				KeyRead = 0;				
				break;	
			}
			else if(PORT1_2_INPUTROW == LOW && PORT1_3_INPUTROW == HIGH && KeyRead ==1)	
			{
				PORT1_4_OUTPUTCOL = OFF;Nop();Nop();Nop();
				PORT1_5_OUTPUTCOL = OFF;Nop();Nop();Nop();
				LeftKey=1;	
				KeyRead = 0;				
				break;
			}
		}	
		if(PORT1_2_INPUTROW == HIGH && PORT1_3_INPUTROW == HIGH ) Cnt++;
		PORT1_4_OUTPUTCOL = ON;Nop();Nop();Nop();
		PORT1_5_OUTPUTCOL = OFF;Nop();Nop();Nop();	
		if(PORT1_2_INPUTROW == HIGH && PORT1_3_INPUTROW == LOW && KeyRead ==1) {

			PORT1_4_OUTPUTCOL = OFF;Nop();Nop();Nop();
			PORT1_5_OUTPUTCOL = OFF;Nop();Nop();Nop();
			MenuKey=1;
			EscKey=1;

			KeyRead = 0;		
			break;	
		}
		else if(PORT1_2_INPUTROW == LOW && PORT1_3_INPUTROW == HIGH && KeyRead ==1) {			

			PORT1_4_OUTPUTCOL = OFF;Nop();Nop();Nop();
			PORT1_5_OUTPUTCOL = OFF;Nop();Nop();Nop();
			EnterKey=1;
			TareKey=1;		
			KeyRead = 0;			
			break;
		}		
		if(PORT1_2_INPUTROW == HIGH && PORT1_3_INPUTROW == HIGH ) Cnt++;
		PORT1_4_OUTPUTCOL = OFF;Nop();Nop();Nop();
		PORT1_5_OUTPUTCOL = OFF;Nop();Nop();Nop();
		if (Cnt==2) {
			KeyRead =1;}
		break;
	}
}
*/

#define	PORT1_2				_RB2
#define PORT1_3				_RB3
#define	PORT1_4				_RB4
#define PORT1_5				_RB5	//Not used

void KeySense()
{
	static char Cnt; 
//	Cnt=0;
	//KeyRead = 0;
	while(1)
	{
		EnterKey = MenuKey = LeftKey = UpKey = EscKey = TareKey = 0;

		if( (PORT1_2 == HIGH) && (PORT1_3 == LOW) && (PORT1_4 == HIGH) && (KeyRead == 1) )
		 {
			UpKey=1;					
			KeyRead = 0;				
			break;	
		}
		else if( (PORT1_2 == LOW) && (PORT1_3 == HIGH) && (PORT1_4 == HIGH) && (KeyRead == 1) )
		{
			TareKey = EnterKey = 1;	
			KeyRead = 0;				
			break;
		}
		else if( (PORT1_2 == HIGH) && (PORT1_3 == HIGH) && (PORT1_4 == LOW) && (KeyRead == 1) )
		{
			MenuKey = EscKey = 1;	
			KeyRead = 0;
			break;
		}		
		
		if( (PORT1_2 == HIGH) && (PORT1_3 == HIGH) && (PORT1_4 == HIGH) ) Cnt++;

		if (Cnt>=8) {
			KeyRead =1;Cnt=0;}
		break;
	}
}

