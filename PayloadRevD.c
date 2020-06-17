
#include <p30f6012A.h>
#include <dsp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <uart.h>
#include <can.h>


#include "General_functions.h"
#include "Disp_LCD.h"

//#include "ADC_Uart.h"

 
#define FCY			7500000						  //7480550 // Fcy = FOCS / 4
#define CANFCY		FCY * 4						  // CAN Clock is multiplied by 4           		
#define BITRATE 	1000000			 			  // 1Mbps
#define NTQ 		15							  // Number of Tq cycles which will make the 
												  //CAN Bit Timing .
#define BRP_VAL		((CANFCY/(2*NTQ*BITRATE))-1)  //Formulae used for C1CFG1bits.BRP 


_FOSC(FRC_PLL4);				// FOSC = ~30mhz 29.9222 = ( 7.37MHZ * 4 Pll) +1.5%   
_FWDT(WDT_OFF);                 /* Turn off the Watch-Dog Timer.  */
_FBORPOR(MCLR_EN & PWRT_OFF);   /* Enable MCLR reset pin and turn off the power-up timers. */
_FGS(CODE_PROT_OFF);


 char *Bufptr;
 char  Buf[20];	
 int value;
 int ADCOUNTS = 0;


 unsigned char count,Samples; 
 unsigned long ADCValue,ADCSampleVal;
 unsigned int *ADC16Ptr;

 unsigned char REMOTEFRAME = 0;
 unsigned char DATAREADY = 0;
 unsigned char ADDATAREADY = 0;


// unsigned int conversionResult[16];
 //volatile unsigned int *ptr;

unsigned int OutData0[4] = {0x0043, 0x004F, 0x004D, 0x0030};  //Data = CAN0           
unsigned int OutData1[2] = {0x0045, 0x046};
unsigned int OutData2[4] = {0x0047, 0x0048, 0x0049, 0x004A};            
unsigned int OutData3[2] = {0x004B, 0x004C};

unsigned int InData0[4] = {0, 0, 0, 0};
unsigned int InData1[2] = {0, 0};
unsigned int InData2[4] = {0, 0, 0, 0};
unsigned int InData3[2] = {0, 0};


void __attribute__((interrupt, no_auto_psv)) _C1Interrupt(void)
{
      
	  IFS1bits.C1IF = 0;         //Clear interrupt flag

      if(C1INTFbits.TX0IF)
      {
       
		 C1INTFbits.TX0IF = 0;  //If the Interrupt is due to Transmit0 of CAN1 Clear the Interrupt

      }  

      else if(C1INTFbits.TX1IF)
      {
        
		C1INTFbits.TX1IF = 0;   //If the Interrupt is due to Transmit1 of CAN1 Clear the Interrupt 

      }  

      if(C1INTFbits.RX0IF)
      {      
        
		C1INTFbits.RX0IF = 0; 	//If the Interrupt is due to Receive0 of CAN1 Clear the Interrupt
		
		if(C1RX0DLCbits.RXRTR ==1)
		{

		  REMOTEFRAME = 1;	

		}

		else
		
		{
		  REMOTEFRAME = 0;	
		
		}	
		
          InData0[0] = C1RX0B1; 
          InData0[1] = C1RX0B2;  	//Move the recieve data from Buffers to InData
          InData0[2] = C1RX0B3;	
          InData0[3] = C1RX0B4;

		  DATAREADY = 1;		
          	
                                  	// turn on the LED
      }

      else if(C1INTFbits.RX1IF)
      {      
        
		C1INTFbits.RX1IF = 0;  	//If the Interrupt is due to Receive1 of CAN1 Clear the Interrupt
        InData1[0] = C1RX1B1;   //Move the data received to Indata Registers
        InData1[1] = C1RX1B2;

        
      }	
}


void __attribute__((__interrupt__)) _ADCInterrupt(void)
{

		ADDATAREADY  = 1;
		ADCOUNTS = ADCBUF0;

//      int i = 0;
  //    ptr = &ADCBUF0;
		
    //  while (i < 16)
      //  {
        //        conversionResult[i++] = *ptr++;
        //}

        //Clear the A/D Interrupt flag bit or else the CPU will
        //keep vectoring back to the ISR
        IFS0bits.ADIF = 0;

}



//--------------------------------------------------------------------------------------------------------------------------
    

void Init_Oscillator(void)
{

 	OSCCON = 0x0700;			// 
 	OSCTUN = 0x0002;		    // Tune 1.5% 

	OSCCON = 0x7846;			// Lock Osc so that accidental overwrite is avoided
	OSCCON = 0x9A57;			// Lock bits

	//while(!OSCCONbits.LOCK);
}



void Init_Device(void)
{

   Init_Oscillator();			// May include Other Def in future 

}




void CAN_Config_CH1(void)

{

	C1CTRLbits.CANCKS = 0;			// Select the CAN Master Clock .
								    // equal to 4 Fcy.(Fcy= 7.5)
	C1CFG1bits.SJW=01;				//Synchronized jump width time is 1 x TQ when SJW is equal to 00
 
	C1CFG1bits.BRP = BRP_VAL;		//((FCY/(2*NTQ*BITRATE))-1) 	

 //	C1CFG2 = 0x03F5;                // SEG1PH=6Tq, SEG2PH=3Tq, PRSEG=5Tq 
        	                        // Sample 3 times
            	                    // Each bit time is 15Tq


	C1CFG2 = 0x04A3;				//Sample 1 times pr 5 + ph1 5 + ph2 5
	///Interrupt Section of CAN Peripheral

	C1INTF = 0;					//Reset all The CAN Interrupts 
    IFS1bits.C1IF = 0;  		//Reset the Interrupt Flag status register
 	C1INTE = 0x00FF;             //Enable Tx0 int only	F4
 	IEC1bits.C1IE = 1;			//Enable the CAN1 Interrupt 


}



void CAN_Config_CH2(void)

{

	C2CTRLbits.CANCKS = 0;			// Select the CAN Module Frequency Here CAN Master Clk is 4
									// times equal to Fcy.(Fcy=7.5Mhz)
	C2CFG1bits.SJW=1;//00;			   	//Synchronized jump width time is 1 x TQ when SJW is equal to 00
 
	C2CFG1bits.BRP = BRP_VAL;		//((FCY/(2*NTQ*BITRATE))-1)  

 
//	C2CFG2 = 0x03F5;                // SEG1PH=6Tq, SEG2PH=3Tq, PRSEG=5Tq
                                	// Sample 3 times
                               		 // Each bit time is 15Tq

	C2CFG2 = 0x04A3;

	//CAN2 Interrupt Section

 	C2INTF = 0;					//Reset all The CAN Interrupts 
 	IFS2bits.C2IF = 0;  			//Reset the Interrupt Flag status register
 //	C2INTE = 0x00F1;               //Enable Rx0 int only//F1
 	IEC2bits.C2IE = 1;				//Enable the CAN2 Interrupt 
}


void SendRTRframe(unsigned int identifier)
{
  
  C1TX0CON = 0x0003;
  C1TX0SID = identifier;   // SID = 01010101010 (0x2AA)
  C1TX0EID = 0x0000;      // EID = 0000000000000000000 (0x00000)
  C1TX0DLC = 0x0380;	 // RTR frame no data 

}


//Functions:
//ADC_Init() is used to configure A/D to convert 16 samples of 1 input
//channel per interrupt. The A/D is set up for a using the ADRC oscillator
//Automatic conversion trigger is applied after 1Tad of sampling time.
//The input pin being acquired and converted is AN8,AN9,AN10,AN11.

void ADC_Init(void)
{
        //ADCON1 Register
        //Set up A/D for Automatic Sampling
        //Use internal counter (SAMC) to provide sampling time
        //Set up A/D conversrion results to be read in unsigned integer format.
        //Set up Sequential sampling for multiple S/H amplifiers
        //All other bits to their default state

		_TRISB8 = 1;		// Input Config
		_TRISB9 = 1;		// Input Config
		_TRISB10 = 1;		// Input Config
		_TRISB11 = 1;		// Input Config

		//ADPCFG Register
        //Set up channels AN8,9,10,11 as analog input
        ADPCFG = 0xFFFF;

        _PCFG8  = 0;
		_PCFG9  = 0;
		_PCFG10 = 0;
		_PCFG11 = 0;	

        ADCON1bits.FORM = 0;		// Unsigned integer O/P
        ADCON1bits.SSRC = 7;		// internal counter ends & conv start
        ADCON1bits.ASAM = 1;		// imm after last conv
									// SIMSAM = 0

        //ADCON2 Register
        //Set up A/D for interrupting after 16 samples get filled in the buffer
        //All other bits to their default state

		ADCON2bits.VCFG = 3;		// External Ref + and Ref-
        ADCON2bits.SMPI = 15;		// Interuupt after 15 convertions
									// Can reduce depend on speed

		//ADCON2bits.CHPS = 1;		// check after

        //ADCON3 Register
        //We will set up the ADRC as the A/D conversion clock
        //so that the A/D converter can operate when the device is in
        //SLEEP mode. Also, 1 Tad period is allocated for sampling time.
        //The conversion rate for the ADRC oscillator is depends on whether
        //the device is a dsPIC30F or dsPIC33F device and also whether the
        //A/D module is a 10-bit or 12-bit A/D converter.
        //Please refer to the device Datasheet for "ADRC" conversion rate.

        ADCON3bits.SAMC = 1;
        ADCON3bits.ADRC = 1;
		ADCON3bits.ADCS = 0;		// 200 KHZ
		
        //ADCHS Register
        //Set up A/D Channel Select Register to convert AN8 on Mux A input
        //of CH0 S/H amplifiers

        // ADCHS = 0x0008;			// channel No

        //ADCSSL Register
        //Channel Scanning is disabled. All bits left to their default state
        ADCSSL = 0x0000;
        
        //Clear the A/D interrupt flag bit
        IFS0bits.ADIF = 0;

        //Set the A/D interrupt enable bit
        IEC0bits.ADIE = 1;

        //Turn on the A/D converter
        //This is typically done after configuring other registers
      //  ADCON1bits.ADON = 1;

}

void ReadADCCounts(unsigned char Channel)

{

   ADCHS = Channel;
   Delay_ms(1);
   	
   ADCON1bits.ADON = 1;
			
	
  ADCSampleVal = 0;

  for(Samples = 0; Samples < 20; Samples++)

  {
	ADCValue = 0; // clear value
	ADC16Ptr = &ADCBUF0;

	ADDATAREADY = 0;
	while(!ADDATAREADY);

	for (count = 0; count < 16; count++) // average the 15 ADC value
	{
	  ADCValue = ADCValue + *ADC16Ptr++;
		
	}

	ADCValue = ADCValue >> 4;		//16 pgm samples 
		
	ADCSampleVal+= ADCValue;
 	 
 }

 ADCSampleVal  = ADCSampleVal / 20;
	
 ADCSampleVal  = ADCSampleVal >> 3;


}



int main(void)

{   	
   unsigned int CH1ADC,CH2ADC,CH3ADC,CH4ADC;

	
	Init_Device();	
	InitLCDDisplay();

	ADC_Init();
	
	ClearLcd();
	lcd_goto(0);
	

	lcd_puts("Adc Test..");
	Delay_1s(1);

			
	ClearLcd();
	lcd_goto(0);
	lcd_puts("C1:");

	lcd_goto(9);
	lcd_puts("C2:");

	lcd_goto(16);
	lcd_puts("C3:");

	lcd_goto(25);
	lcd_puts("C4:");

	
	do{
    
	ReadADCCounts(8);
    CH1ADC = (unsigned int)ADCSampleVal;
	
	ReadADCCounts(9);
    CH2ADC = (unsigned int)ADCSampleVal;
	
	ReadADCCounts(10);
    CH3ADC = (unsigned int)ADCSampleVal;
	
	ReadADCCounts(11);
    CH4ADC = (unsigned int)ADCSampleVal;
	

	sprintf(Buf,"%04d",CH1ADC);
	lcd_goto(3);
	lcd_putch(Buf[0]);
	lcd_putch(Buf[1]);
	lcd_putch(Buf[2]);
	lcd_putch(Buf[3]);

	sprintf(Buf,"%04d",CH2ADC);
	lcd_goto(12);
	lcd_putch(Buf[0]);
	lcd_putch(Buf[1]);
	lcd_putch(Buf[2]);
	lcd_putch(Buf[3]);

	sprintf(Buf,"%04d",CH3ADC);
	lcd_goto(19);
	lcd_putch(Buf[0]);
	lcd_putch(Buf[1]);
	lcd_putch(Buf[2]);
	lcd_putch(Buf[3]);

	sprintf(Buf,"%04d",CH4ADC);
	lcd_goto(28);
	lcd_putch(Buf[0]);
	lcd_putch(Buf[1]);
	lcd_putch(Buf[2]);
	lcd_putch(Buf[3]);

	Delay_ms(20);

	}

	while(1);


}	

 
 






