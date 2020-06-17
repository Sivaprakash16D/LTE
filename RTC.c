#include <p30f6012A.h>
#include <string.h>
#include <stdlib.h>

#include "Disp_LCD.h"
#include "General_functions.h"


#define	RTCSDA				_LATC13
#define RTCSDA_PORT			_RC13
#define	RTCSCL				_LATC14

#define	RTCSDA_Direction	_TRISC13
#define	RTCSCL_Direction	_TRISC14

//----------------------------------------------------------------

#define		WRITE_C		0xde 		//Command for write
#define		READ_C		0xdf		//Command for read
#define		ACK_READ	0xdf		//Command to get acknowledge

//----------------------------------------------------------------

#define HIGH	1
#define LOW		0
#define WRITE	0
#define READ	1
#define ADDRESS_SECOND	0x00
#define ADDRESS_MINUTE	0x01
#define ADDRESS_HOUR	0x02

#define ADDRESS_DATE	0x03
#define ADDRESS_MONTH	0x04
#define ADDRESS_YEAR	0x05
#define ADDRESS_WEEKDAY	0x06

#define ADDRESS_STATUS  0x07
#define RTC_WRITE_EN	0x10
#define ADDRESS_RTCRAM	0x12

struct struct_rtc
{
	char Hour;
	char Minute;
	char Second;
	char Date;
	char Month;
	int Year;
	char WeekDay;
    char AMPM;	
    int  HoursNMinute;
    int  DateNMonth;
    int  Seconds;
};

struct struct_rtc RTC;


void Init_SCL_As_Output(void)
{

	RTCSCL_Direction = 0;			//Set Clock as output
}


void Set_SDA_As_Output(void)
{

	RTCSDA_Direction = 0;			//Set as output
}


/* Sets input mode to SDA line */
void Set_SDA_As_Input(void)
{
	
	RTCSDA_Direction = 1;			//Set as input
}


void IICClockDelay(void)
{

	char	a;
	for(a=0;a<60;a++);				// increase 3 times for 30mhz
}


void StartBit(void)
{
	Set_SDA_As_Output();

	RTCSCL = 1;
	IICClockDelay();

	RTCSDA	= 1;
	IICClockDelay();
	RTCSDA	= 0;
	IICClockDelay();

	RTCSCL = 0;		
	IICClockDelay();
}

/* Get acknowledge from slave */
char SlaveAck(void)
{
	char	a;
	Set_SDA_As_Output();
	IICClockDelay();

	RTCSCL	= 0;
	IICClockDelay();

	RTCSDA	= 1;
	IICClockDelay();

	RTCSCL = 1;
	IICClockDelay();

	Set_SDA_As_Input();

	IICClockDelay();
	a   = RTCSDA_PORT;
	IICClockDelay();

	RTCSCL = 0;
	IICClockDelay();

	return(a);
}


/* No acknowledge from slave */
void SlaveNoAck(void)
{
	Set_SDA_As_Output();
	RTCSDA	= 1;
	IICClockDelay();
	RTCSCL	= 1;
	IICClockDelay();
	RTCSCL = 0;
	IICClockDelay();
}


/* Send One Byte to RTc */
void SendOneByteIIC(unsigned char d)
{
	char	a;
	Set_SDA_As_Output();

	for(a=0;a<8;a++)		//send 8 bits 
	{	
		RTCSCL = 0;			//clear SCL 
		IICClockDelay();		//delay

		if(d & (0x80 >> a))
		{
			RTCSDA = 1;		//set SDA 
		}
		else
		{
			RTCSDA = 0;		//clear SDA
		}

		IICClockDelay();				

		RTCSCL	= 1;			//set SCL
		IICClockDelay();
	}
}


void StopBit(void)
{
	Set_SDA_As_Output();

	RTCSDA	= 0;
	IICClockDelay();
	RTCSCL	= 1;
	IICClockDelay();

	RTCSDA = 1;		
}


void PollAck(void)
{
	char	a;
	for(a=0;a<0x40;a++)
	{
		StartBit();			//Give start bit
		SendOneByteIIC(ACK_READ);	//Send the read Acknowledgement command
		if(SlaveAck())			//Get the acknowledge from slave
		{	//acknowledge received
			break;
		}
	}
}


void WriteByteRTC(unsigned char Address, unsigned char Data)
{
//	WriteByteRTC(0x3F,0x06);
	StartBit();						//Give start bit
	SendOneByteIIC(WRITE_C);		//Send the write command
	SlaveAck();						//Get the acknowledge from slave
//	SendOneByteIIC(0x00);			//16 bit address
//	SlaveAck();	
	SendOneByteIIC(Address);		//Send the address
	SlaveAck();						//Get the acknowledge from slave
	SendOneByteIIC(Data);			//Send the Data
	SlaveAck();						//Get the acknowledge from slave
	StopBit();						//Give stop bit
	PollAck();
}


void WriteToRTC_Time()
{
	WriteByteRTC(ADDRESS_STATUS,RTC_WRITE_EN);		
	WriteByteRTC(ADDRESS_SECOND,RTC.Second); 	// write the Second value	
	WriteByteRTC(ADDRESS_STATUS,RTC_WRITE_EN);
	WriteByteRTC(ADDRESS_MINUTE,RTC.Minute); 	// write the Minute value	
	WriteByteRTC(ADDRESS_STATUS,RTC_WRITE_EN);
	WriteByteRTC(ADDRESS_HOUR,RTC.Hour); 		// write the Hour
}


void WriteToRTC_Date()
{
	WriteByteRTC(ADDRESS_STATUS,RTC_WRITE_EN);
	WriteByteRTC((char)ADDRESS_DATE,(char)(RTC.Date));		// write the Date	
	WriteByteRTC(ADDRESS_STATUS,RTC_WRITE_EN);		
	WriteByteRTC(ADDRESS_WEEKDAY,RTC.WeekDay); 	// write the Weekday(sunday-staturday)	
	WriteByteRTC(ADDRESS_STATUS,RTC_WRITE_EN);
	WriteByteRTC((char)ADDRESS_MONTH,(char)(RTC.Month|0x80)); // write the Month	
	WriteByteRTC(ADDRESS_STATUS,RTC_WRITE_EN);
	WriteByteRTC((char)ADDRESS_YEAR,(char)(RTC.Year)); 		// write the year
}

void WriteToRtcData()
{
 //   Set_SCL_As_Output();
	WriteByteRTC(ADDRESS_STATUS,RTC_WRITE_EN);
	WriteByteRTC(ADDRESS_RTCRAM,'X'); // write the Second value
}

char AsciitoBcd(char *Buff)
{
	return((char)(Buff[1]-48+(((Buff[0]-48)<<4)&0xf0)));
}


/*void SetClock()
{
  //  char * Sbuff;


    Sbuff[0] = Sbuff[1] = '0';
    Sbuff[2] = Sbuff[3] = '0';
	Sbuff[4] = Sbuff[5] = '0';

	WriteByteRTC(0x3F,0x06);
	RTC.Hour  = AsciitoBcd(Sbuff);
	RTC.Hour |= 0x80;				// setting as 24 hr format
	RTC.Minute= AsciitoBcd(Sbuff+2);
	RTC.Second= AsciitoBcd(Sbuff+4);
	WriteToRTC_Time();
}*/


/* Read One Byte (bit by bit) from RTC */
unsigned char ReadOneByteIIC()
{
	char	a;
	unsigned char b;

	b=0;
	Set_SDA_As_Input();

	for(a=0;a<8;a++)
	{
		RTCSCL = 1;
		IICClockDelay();

		if(RTCSDA_PORT)
		{
			b |= (0x80 >> a);
		}

		IICClockDelay();
		RTCSCL	= 0;
		IICClockDelay();
	}
	return(b);
}


/* Read Byte from RTC */
unsigned char ReadByteRTC(unsigned char Address)
{
	unsigned char	a;
									//Write Address
	StartBit();						//Give start bit
	SendOneByteIIC(WRITE_C);		//Send the write command
	SlaveAck();						//Get the acknowledge from slave
//	SendOneByteIIC(0x00);			//16 bit address
//	SlaveAck();	
	SendOneByteIIC(Address);		//Send the address
	SlaveAck();						//Get the acknowledge from slave
	
									//Read a byte
	StartBit();						//Give start bit
	SendOneByteIIC(READ_C);			//Send the read command
	SlaveAck();						//Get the acknowledge from slave
	a=ReadOneByteIIC();				//Read the byte
	SlaveNoAck();
	StopBit();						//Give stop bit
	return (a);
}


char BcdtoBinary(char BCDValue)
{
	char Buff[3];

	Buff[2]='\0';
	Buff[0]= (char)(((BCDValue>>4)&0x0f)+48);
	Buff[1]= (char)((BCDValue&0x0f)+48);
	return((char)(atoi(Buff)));
}

char BinaryToBCD(char Value)
{
	char Digi1,Digi2,BCDValue;

	Digi2 = Value / 10;
	Digi1 = Value - Digi2 * 10;
	BCDValue = Digi2 << 4 | Digi1;
	return((char)BCDValue);
}

 
void SetDate(unsigned char Bdate, unsigned char Bmonth,unsigned char Byear)		
{
	WriteByteRTC(0x3F,0x06);
   	RTC.Date	= Bdate;
	RTC.Month	= Bmonth;
	RTC.Year	= Byear;
	WriteToRTC_Date();
}


void SetTime(unsigned char BHour,unsigned char BMin,unsigned char BSec)
{
 	WriteByteRTC(0x3F,0x06);
	RTC.Hour  = BHour;
	RTC.Hour |= 0x80;				// setting as 24 hr format
	RTC.Minute= BMin;
	RTC.Second= BSec;
	WriteToRTC_Time();
}

unsigned char* ReadClock24Hr()
{
   	RTC.Second	= ReadByteRTC(ADDRESS_SECOND);
   	RTC.Minute	= ReadByteRTC((ADDRESS_MINUTE));
   	RTC.Hour	= ReadByteRTC((ADDRESS_HOUR));
   	RTC.Second 	= BcdtoBinary((char)(RTC.Second&0x7f));
   	RTC.Minute	= BcdtoBinary((char)(RTC.Minute&0x7f));
   	RTC.Hour	= BcdtoBinary((char)(RTC.Hour&0x3f));

	RTC.HoursNMinute = RTC.Hour * 256 + RTC.Minute;
	RTC.Seconds = (unsigned int) RTC.Second;		
 	return(&RTC.Hour);
}

unsigned char* ReadClock12Hr()
{  
   	RTC.Second	= ReadByteRTC(ADDRESS_SECOND);
   	RTC.Minute	= ReadByteRTC((ADDRESS_MINUTE));
  	RTC.Hour	= ReadByteRTC((ADDRESS_HOUR));
   	RTC.Second 	= BcdtoBinary((char)(RTC.Second&0x7f));
   	RTC.Minute	= BcdtoBinary((char)(RTC.Minute&0x7f));
   	RTC.Hour	= BcdtoBinary((char)(RTC.Hour&0x3f));
   
	if(RTC.Hour>=12)
	{
		if(RTC.Hour!=12)
		{	RTC.Hour-=12;
			RTC.AMPM =1; 	// time in pm
		}

		else 
		{
			RTC.AMPM=0;
			if(RTC.Hour==0)
			RTC.Hour=12;	// time in am
		}
	}

  return(&RTC.Hour);

}
  
 
unsigned char* ReadDate()
{  
	RTC.Date		= ReadByteRTC((ADDRESS_DATE));
	RTC.WeekDay 	= ReadByteRTC((ADDRESS_WEEKDAY));
	RTC.Month		= ReadByteRTC((ADDRESS_MONTH));
	RTC.Year		= ReadByteRTC((ADDRESS_YEAR));
	
	RTC.Date		= BcdtoBinary((char)(RTC.Date&0x3f));
	RTC.WeekDay	= BcdtoBinary((char)(RTC.WeekDay&0x07));
	RTC.Month		= BcdtoBinary((char)(RTC.Month&0x3f));
	RTC.Year 		= BcdtoBinary(RTC.Year)+2000;		// year is 0 to 99 only. add 2000 to get current year
	
	RTC.DateNMonth = RTC.Date * 256 + RTC.Month ;	
  return(&RTC.Date);
}


/*
void DisplayTime()
{
   char LcdBuf[15];

	ReadClock();
	sprintf(LcdBuf,"%02u",RTC.Hour);
  	lcd_goto(5);
   
    lcd_putch(LcdBuf[0]);
	lcd_putch(LcdBuf[1]);
	lcd_puts(":");
    
	sprintf(&LcdBuf[2],"%02u",RTC.Minute);
	lcd_putch(LcdBuf[2]);
	lcd_putch(LcdBuf[3]);
	lcd_puts(":");

    sprintf(&LcdBuf[4],"%02u",RTC.Second);
	lcd_putch(LcdBuf[4]);
	lcd_putch(LcdBuf[5]);

	Delay_ms(1);	
	
}

void TestTime()
{
  unsigned char c = '0';


  lcd_goto(5);
  c = ReadByteRTC(ADDRESS_RTCRAM);
  lcd_putch(c);
  Delay_1s(1);
}

*/

void ReadRTC_DateTime()
{
	ReadDate();
	ReadClock24Hr();	
}	
