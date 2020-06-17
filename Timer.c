#include <p30f6012A.h>
#include <dsp.h>
#include <stdlib.h> 
#include "Timer.h"                               

#define Timer1Fcy	7480550	// (Fosc = 29922200 Mhz / 4)

#define ErrorCount 	5
#define Delay10uS_count    74   //((Timer1Fcy / 1000000) * 10) 
#define Delay_1mS_Cnt      7480 //((Timer1Fcy / 1000000) * 1000)




void Delay_10us(unsigned char Delayval) // 1.1 micro error
{
    	
  while(Delayval--)
							// Timer 1 Fcy /4 clock, 1:1 prescalar					
  {
    T1CONbits.TON = 0;		// Stop Timer

  	TMR1  = 0;				// Run from 0 value
  	PR1	=  Delay10uS_count;	 
  
 	T1CONbits.TON = 1; 		// Run Timer

   while(TMR1 != PR1 );		// Wait till timer reg reach Period value
 }

}



void Delay1_1s(unsigned char Delayval)
{
    while(Delayval--){
        Delay_ms(250);
        Delay_ms(250);
        Delay_ms(250);
        Delay_ms(250);
    }
}
