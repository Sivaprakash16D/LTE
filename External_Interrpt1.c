#include <p30f6012A.h>
#include <string.h>
#include "External_Interrpt.h"

extern unsigned char InterruptFlag;
extern unsigned char Delayval;
void __attribute__((__interrupt__)) _INT0Interrupt(void)
{
/*
	if(IFS0bits.INT0IF)
	{	
		EEROM_RAMVALUE = EEROM_RAMVALUE + 2;		
		IFS0bits.INT0IF = 0;
	}
		    //Clear the INT0 interrupt flag or else
                               //the CPU will keep vectoring back to the ISR
*/
	
}
void __attribute__((__interrupt__)) _T1Interrupt(void)
{
/* Insert ISR Code Here*/
 if (Delayval > 0 ) Delayval--;


/* Clear Timer1 interrupt */
IFS0bits.T1IF = 0;
}

void __attribute__((__interrupt__)) _INT1Interrupt(void)	// interrupt one 
{
	if(IFS1bits.INT1IF)	// interrupt one status flag
	{	
		InterruptFlag++;
	//	EEPROMINC_VAL++;
		IFS1bits.INT1IF = 0;		
	}
		
}
void __attribute__((__interrupt__)) _INT2Interrupt(void)		// interrupt two
{
	if(IFS1bits.INT2IF)		// interrupt two  status flag
	{	
		InterruptFlag++;
	//	EEPROMINC_VAL++;
		IFS1bits.INT2IF = 0;		
	}
		
}
void __attribute__((__interrupt__)) _INT3Interrupt(void)		// interrupt three 
{
	if(IFS2bits.INT3IF)		// interrupt three status flag
	{	
		InterruptFlag++;
	//	EEPROMINC_VAL++;
		IFS2bits.INT3IF = 0;		
	}
		
}
void __attribute__((__interrupt__)) _INT4Interrupt(void)		// interrupt four
{
	if(IFS2bits.INT4IF)		// interrupt four status flag
	{	
		InterruptFlag++;
	//	EEPROMINC_VAL++;
		IFS2bits.INT4IF = 0;		
	}
		
}
