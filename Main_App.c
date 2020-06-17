/*****************************************************************************************
Project Name:   LTE Modem addon for MW5100
Software Rev:   Version 5.0 (Common Software for IoT Interaction)
Description :   * Quectel EC25 module used for LTE and as well as GPS. (AT commands send through UART)
                * Memory has divided into three banks and user can able to modify or configure it.
                * Three types of Records will be stored and sent to server simultaneously.
                * One record as immediate data that will not be stored while receive. It will directly sent to server once received
******************************************************************************************/

#include "p30fxxxx.h"
#include "libpic30.h"	//EEROM
#include "dsp.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "Main_App.h"
#include "LCD_DISP.h"
#include "E2C_1024.h"
#include "Keypad.h"


//#define FCY			7500000

_FOSC(FRC_PLL4);				// FOSC = ~30mhz 29.9222 = ( 7.37MHZ * 4 Pll) +1.5%   
_FWDT(WDT_OFF);                 /* Turn off the Watch-Dog Timer.  */
_FBORPOR(MCLR_EN & PWRT_OFF);   /* Enable MCLR reset pin and turn off the power-up timers. */
_FGS(CODE_PROT_OFF);

#define TESTING

//-------------
#define FOSCOSEL 0x000001
#define FPOR 0x000007

void Init_Oscillator(void)
{
	int Counter;
 	OSCCON = 0x0700;			// FOSC = ~30mhz 29.9222 = ( 7.37MHZ * 4 Pll) +1.5%
// 	OSCTUN = 0x0002;		    // Tune 1.5% 
	for(Counter=0;Counter>=1000;Counter++){
	}

	OSCCON = 0x7846;			// Lock Osc so that accidental overwrite is avoided
	OSCCON = 0x9A57;			// Lock bits
//	while(!OSCCONbits.LOCK);
}
char Modem_State_Buff[10] = {0};
extern char Send_Buff1[130];
char HW_reset_Counter = 0;

int main(void)
{
//    int a =0;
	PowerOn();
	LcdDisplay(TOP,   "  GPRS Module   ");
	LcdDisplay(BOTTOM,"   Ver 5.4.3    ");
	//LcdDisplay(BOTTOM,"under developmnt");
	Delay1_1s(1);
    Version = 531;
//     for(a = 0;a<500;a++)
//    {
//        Write_E2(a,(char*)&a,sizeof(a));Delay(2);
//    }
	Read_E2((char*)&MemoryCheck,EE_MemoryCheck,1);Delay(2);
	if(MemoryCheck != 71){
		LoadDefault();
		ResetValues();
		APN_Length = 8;
		memcpy(APN_Name,"internet",APN_Length);
	}
	else
        ReadSettings();
  
   
   
/*  For LTE Modem Initialization Purpose    */
    LcdDisplay(TOP,"                ");
	LcdDisplay(BOTTOM," Please wait.   ");
//	Delay1_1s(1);     
    LcdDisplay(BOTTOM," Please wait..  ");
//    Delay1_1s(2);
    LcdDisplay(BOTTOM," Please wait... ");
//    Delay1_1s(2);
/*  For LTE Modem Initialization Purpose    */

    if(Rec[0].DisplayNo > Rec[0].MaxRecLimit)
        Rec[0].OverFlowFlag = 1;
    
    if(Rec[1].DisplayNo > Rec[1].MaxRecLimit)
        Rec[1].OverFlowFlag = 1;
    
    if(Rec[2].DisplayNo > Rec[2].MaxRecLimit)
        Rec[2].OverFlowFlag = 1;

    GPRS_Flags1.Modem_Rdy = 1;

    while((ReceiveBuff1[2] != 'R') && (ReceiveBuff1[3] != 'D') && (ReceiveBuff1[4] != 'Y'))
    {
        if((Alivecounter >= 30000) && (Alivecounter < 30010))
        {
            memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
            _RD9 = 0;
            Com1StringSend("AT+QPOWD=0",10);		//AT+QPOWD=0 Quick Powerdown
            HW_reset_Counter++;
            Com1Bytesend(0x0d);
            Com1Bytesend(0x0a);
        	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
        }
        else if(Alivecounter >= 60000)
        {
            GPRS_Flags1.Modem_Rdy = 0;
            Alivecounter = 0;
            LcdDisplay(BOTTOM,"Modem Not Ready ");
            LcdDisplay(TOP,"                ");
            Delay1_1s(1);
            break;
        }
    }
    
    GPRS_Flags1.Modem_Rdy = 0;       
	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;

    Modem_Init();  //   Includes echo command, Network search command

	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
    
    sprintf(LcdDispBuff,"STS%02d1",Version);
    Com2StringSend_with_LRC(LcdDispBuff,6); //   To Indicate like LTE module Powered ON with version indication
	memset(LcdDispBuff,' ',32);
	while(1)                                       //   Super loop
	{
		_RD9 = 0;                                  //   GPRS Power pin
        MainScreenDisplay();
		Do_Sim_Config();
        Data_Receiving();
        Reset_LTE_Modem();
        Data_Sending();
        GPRS_Close_State();
        //GPS_Process();
        
//        if(Modem_Response_Flag == 9)
//            Com2StringSend("ubuwfhyuhuiuh",13);
    }
}

void Modem_Init()
{
     Com1StringSend("AT&F",4);                           //  To Factory  reset
    Com1Bytesend(0x0D);
    Com1Bytesend(0x0A);
    Delay(10000);
    
    Com1StringSend("ATE0",4);                           //  To Stop Echo
    Com1Bytesend(0x0D);
    Com1Bytesend(0x0A);
    Delay(10000);

    //Com2StringSend(ReceiveBuff1,ReceiveCounter1);
	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
    
    Com1StringSend("AT+QCFG=\"nwscanmode\",0",22);      //  This is used to set the mode in 2g or 3g or 4g(0-AUTO,1-GSM,2-WCDMA,3-LTE) - Now it is set as Auto mode
    Com1Bytesend(0x0d);
    Com1Bytesend(0x0a);
    Delay(10000);
    
    //Com2StringSend(ReceiveBuff1,ReceiveCounter1);
	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
    
    Com1StringSend("AT+QSIMSTAT=1",13);                 //  Enabling the detection of SIM whether it is inserted or not
	Com1Bytesend(0x0D);
	Com1Bytesend(0x0A);
	Delay(20000);
    Delay1_1s(2);
    //Com2StringSend(ReceiveBuff1,ReceiveCounter1);
	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
    
    Com1StringSend("AT+CMGF=1",9);                     //   This is used to set SMS in text mode,if = 0 it is in PDU mode
	Com1Bytesend(0x0D);
	Com1Bytesend(0x0A);
	Delay(10000);
    Delay1_1s(2);

    //Com2StringSend(ReceiveBuff1,ReceiveCounter1);
	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
    
    Com1StringSend("AT+CSCS=\"GSM\"",13);              //    This is used to set character set in English
	Com1Bytesend(0x0d);
	Com1Bytesend(0x0a);
	Delay(10000);
    

    //Com2StringSend(ReceiveBuff1,ReceiveCounter1);
	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
    
    Com1StringSend("AT+CNMI=2,1,0,0,0",17);              //    New Message Indication
	Com1Bytesend(0x0d);
	Com1Bytesend(0x0a);
	Delay(10000);
    

    //Com2StringSend(ReceiveBuff1,ReceiveCounter1);
	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;

	Com1StringSend("AT&W",4);                           //  Save and Exit
	Com1Bytesend(0x0D);
	Com1Bytesend(0x0A);
	Delay(10000);

    //Com2StringSend(ReceiveBuff1,ReceiveCounter1);
	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
}

void Mode_Check(void)
{
    if(Modem_Request == 0)
    {
        k = 0;
        memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;    
        Com1StringSend("AT+QNWINFO",10);//this is used to set the mode in 2g or 3g or 4g(0-AUTO,1-GSM,2-WCDMA,3-LTE))
        Com1Bytesend(0x0d);
        Com1Bytesend(0x0a);
        Modem_Request=1;
    }
    if(Modem_Request == 1)
    {
        if(ReceiveCounter1 >= 20)
        {
            if(!strncmp(ReceiveBuff1+10,": \"",3))
            {
                for(i=0;i<100;i++)
                {
                    if(ReceiveBuff1[13+i] != '\0')
                    {
                        if(ReceiveBuff1[13+i] != '"')
                        {
                            Buffer_Value[k++] = ReceiveBuff1[13+i];
                        }
                        else
                        {
                            memset(ReceiveBuff1,' ',350);
                            Modem_Request = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
}

 void Data_Sending()
{
     
    if(Modem_Init_State == MODEM_OK)
    {
        GPRS_Update();
        Modem_Init_Cntr = 0;
//  Below function is used to check the signal level is '0' more than one minute reset counter will set
        if(SignalLevelCheck == 0)
        {
            SignalLevelTimer++;
            if(SignalLevelTimer > 18000)	
            {
                SignalLevelTimer = 0;
                GPRS_ResetFlag = 1;
               // SignalLevel_Reset = 1;
            }
        }
        else
          SignalLevelTimer = 0;
//  Above function is used to check the signal level is '0' more than one minute reset counter will set
        
//  below condition is used to check the signal level if the data not sent to the server.
        if((Alivecounter > 30000) && ((GPRS_Flag == 0)||(SMS_Flag == 0)))
        {
            Alivecounter = 0;
            CheckSignalStrength();
        }
//  above condition is used to check the signal level if the data not sent to the server.        
    }
    else
    {
        SignalLevelTimer = 0;
        Modem_Init_Cntr++;
        if(Modem_Init_Cntr >= 36000)    //12000
        {
            Modem_Init_Cntr = 0;
            GPRS_ResetFlag = 1;
        }
    }
}

void Reset_LTE_Modem(void)
{
    if(GPRS_ResetFlag == 1)
    {
        ImmediateSentFlag = 0;
        //SentDataCopied = 0;
        HW_reset_Counter = HW_reset_Counter + 1;
//        sprintf(Send_Buff1,"HW=%u         ",HW_reset_Counter);
//        LcdDisplay(TOP, Send_Buff1);
//        Delay1_1s(3);
        if((HW_reset_Counter == 5 )||(HW_reset_Counter == 15 )||(HW_reset_Counter == 45 )||(HW_reset_Counter == 105 ))
        {
            SignalLevel_Reset = 0;
            LcdDisplay(TOP," HW Reset Done  ");
            Delay1_1s(2);
            GPRS_HW_RESET = 0;Nop();                //  4G modem - Hardware Resetting - RESET Pin
            Delay1_1s(1);
            GPRS_HW_RESET = 1;Nop();
            Delay1_1s(10);                          //  Recommended 9 seconds
            if(HW_reset_Counter >= 105)
                HW_reset_Counter = 0;
        }
        else
        {
            memcpy(LcdDispBuff+16,"SIM   ",6);
            LcdDisplay(BOTTOM,LcdDispBuff+16);
            memcpy(LcdDispBuff,"   Reset Done   ",16);
            LcdDisplay(TOP,LcdDispBuff);
            Com1StringSend("AT+QPOWD=0",10);		//Software Reset through AT Command AT+QPOWD=0 - Quick Powerdown
            Com1Bytesend(0x0d);
            Com1Bytesend(0x0a);
            Delay1_1s(10); 
        }        
        SignalLevelCheck = 0;
        GPRS_ResetFlag = 0;
        Modem_Init_State = 0;
        Modem_Cmd_Sent = 0;
        memset(ReceiveBuff1,' ',350);
        GPRSTimer = 0;
        GPRS_Flags1.SignalOk = 0;
        SignalLevel = 0;
        SignalTimer = 0;
        
        QIACT_Flag = GPRS_Flag = 0;
        SMS_Flag = 0;
        Modem_Init_Cntr = 0;
		SignalLevelTimer = 0;    

        SMS_Flag = SMS_Response_Flag = 0;

        memset(Buffer_Value,' ',20);
        Modem_Init();                                           //   Includes echo command, Network search command
        Delay1_1s(1);
        Do_Sim_Config();
    }   
}
void Do_Sim_Config(void)
{
	static unsigned int Modem_Tmr = 0;
	if(Modem_Init_State != MODEM_OK)
	{
        if(Modem_Init_State == 0)
		{
			if(Modem_Cmd_Sent == 0)
			{
				memcpy(Modem_State_Buff,"SIMchk",6);
				Com1StringSend("AT+QSIMSTAT?",12);
                Com1Bytesend(0x0d);
                Com1Bytesend(0x0a);
                memset(ReceiveBuff1,' ',350);
				ReceiveCounter1 = 0;
				Modem_Cmd_Sent = 1;
			}
			else
			{
				if((ReceiveBuff1[2] == '+') && (ReceiveBuff1[3] == 'Q') && (ReceiveBuff1[4] == 'S') && 
					(ReceiveBuff1[9] == 'A') && (ReceiveBuff1[11] == ':') && (ReceiveBuff1[13] == '1') && (ReceiveBuff1[15] == '1') && (ReceiveBuff1[16] == 0x0d) && (ReceiveBuff1[17] == 0x0a))
				{
					Modem_Cmd_Sent = 0; 
					Modem_Init_State = 1;
                    SimErrorFlag = 0;
				}
				else if((ReceiveBuff1[2] == '+') && (ReceiveBuff1[3] == 'Q') && (ReceiveBuff1[4] == 'S') && 
					(ReceiveBuff1[9] == 'A') && (ReceiveBuff1[11] == ':') && (ReceiveBuff1[13] == '1') && (ReceiveBuff1[15] == '0') && (ReceiveBuff1[16] == 0x0d) && (ReceiveBuff1[17] == 0x0a))
				{
					Modem_Cmd_Sent = 0; 
					Modem_Init_State = 0;
                    SimErrorFlag = 1;
				}
			}
		}
        else if(Modem_Init_State == 1)
		{
			if(Modem_Cmd_Sent == 0)
			{
				memcpy(Modem_State_Buff,"PINchk",6);
				memset(ReceiveBuff1,' ',350);
				ReceiveCounter1 = 0;
				Com1StringSend("AT+CPIN?",8);
				Com1Bytesend(0x0d);
				Com1Bytesend(0x0a);
				Modem_Cmd_Sent = 1;
				Delay(5000);
			}
			else
			{
				if((ReceiveBuff1[9] == 'R') && (ReceiveBuff1[12] == 'D') && (ReceiveBuff1[13] == 'Y') && 
					(ReceiveBuff1[18] == 'O') && (ReceiveBuff1[19] == 'K') && (ReceiveBuff1[20] == 0x0d) && (ReceiveBuff1[21] == 0x0a))
				{
					Modem_Cmd_Sent = 0; 
					Modem_Init_State = 2;
				}
			}
		}
		else if(Modem_Init_State == 2)
		{
			if(Modem_Cmd_Sent == 0)
			{
				memcpy(Modem_State_Buff,"SIMREG",6);
				memset(ReceiveBuff1,' ',350);
				ReceiveCounter1 = 0;
				Com1StringSend("AT+CREG?",8);
				Com1Bytesend(0x0d);
				Com1Bytesend(0x0a);
				Modem_Cmd_Sent = 1;
				Delay(5000);
			}
			else
			{
				if(((ReceiveBuff1[11] == '1') || (ReceiveBuff1[11] == '5')) && 
					(ReceiveBuff1[16] == 'O') && (ReceiveBuff1[17] == 'K') && (ReceiveBuff1[18] == 0x0d) && (ReceiveBuff1[19] == 0x0a))
				{
					Modem_Cmd_Sent = 0; 
					Modem_Init_State = 3;
				}
                else if(((ReceiveBuff1[11] == '0') || (ReceiveBuff1[11] == '2') || (ReceiveBuff1[11] == '3') || (ReceiveBuff1[11] == '4')) && 
					(ReceiveBuff1[16] == 'O') && (ReceiveBuff1[17] == 'K') && (ReceiveBuff1[18] == 0x0d) && (ReceiveBuff1[19] == 0x0a))
				{
					Modem_Cmd_Sent = 0; 
					Modem_Init_State = 2;
				}
			}
		}
		else if(Modem_Init_State == 3)
		{
			if(Modem_Cmd_Sent == 0)
			{
				memcpy(Modem_State_Buff,"APNSET",6);
				memset(ReceiveBuff1,' ',350);
				ReceiveCounter1 = 0;
				Com1StringSend("AT+QICSGP=1,1,\"",15);
				if(GPRS_Setup.MobileOptr == 0)                  //  Airtel
				{
					Com1StringSend("airtelgprs.com\"",15);
				}
				else if(GPRS_Setup.MobileOptr == 1)             //  TATA Docomo
				{
					Com1StringSend("TATA.DOCOMO.INTERNET\"",21);
				}
				else if(GPRS_Setup.MobileOptr  == 2)            //  Idea
				{
					Com1StringSend("internet\"",9);
				}
				else if(GPRS_Setup.MobileOptr  == 3)            //  Aircel
				{
					Com1StringSend("aircelgprs.pr\"",14);
				}
				else if(GPRS_Setup.MobileOptr  == 4)            //  BSNL
				{
					Com1StringSend("bsnlnet\"",8);
				}
				else if(GPRS_Setup.MobileOptr  == 5)            //StarHub Mobile Pte Ltd (StarHub)
				{
					Com1StringSend("shwapint\"",9);
				}
				else if(GPRS_Setup.MobileOptr  == 6)            //Singapore Telecom Mobile Pte Ltd (SingTel)	APN settings for prepaid:
				{
					Com1StringSend("sunsurf\"",8);  
				}
				else if(GPRS_Setup.MobileOptr  == 7)            //StarHub Mobile Pte Ltd (StarHub2)
				{
					Com1StringSend("shwap\"",6);
				}
				else if(GPRS_Setup.MobileOptr  == 8)            //Singapore Telecom Mobile Pte Ltd (SingTel)	APN settings for prepaid
				{
					Com1StringSend("hi-internet\"",12);
				}
				else if(GPRS_Setup.MobileOptr  == 9)            //Singapore Telecom Mobile Pte Ltd (SingTel)	 APN settings for postpaid
				{
					Com1StringSend("e-ideas\"",8);
				}
				else if(GPRS_Setup.MobileOptr  == 10)           //  M1 postpaid
				{
					Com1StringSend("miworld\"",8);
				}
				else if(GPRS_Setup.MobileOptr  == 11)           //  M1 prepaid
				{
					Com1StringSend("miworldcard\"",12);
				}
				else if(GPRS_Setup.MobileOptr  == 12)           //  Vodafone
				{
					Com1StringSend("www\"",4);
				}
				else if(GPRS_Setup.MobileOptr  == 13)           //  Jio
				{
					Com1StringSend("jionet\"",7);
				}
				else if(GPRS_Setup.MobileOptr  == 14)	//Singapore Telecom Mobile Pte Ltd (SingTel)	 APN settings for postpaid: 
				{
					Com1StringSend(APN_Name,APN_Length);
					Com1StringSend("\"",1);
					Com2StringSend(APN_Name,APN_Length);
					Com2StringSend("\"",1);
				}
				Com1StringSend(",\" \",\" \",1",10);
				Com1Bytesend(0x0d);
				Com1Bytesend(0x0a);
				Delay(5000);
				Modem_Cmd_Sent = 1;
			}
			else
			{
				if((ReceiveBuff1[2] == 'O') && (ReceiveBuff1[3] == 'K') && (ReceiveBuff1[4] == 0x0d) && (ReceiveBuff1[5] == 0x0a))
				{
					memcpy(Modem_State_Buff,"INIOK",6);
					Modem_Cmd_Sent = 0; 
					Modem_Init_State = MODEM_OK;
				}
			}
		}
		if(Modem_Cmd_Sent == 1)
		{
			if(Modem_Tmr++ > 2000){
				Modem_Tmr = 0;
				Modem_Cmd_Sent = 0; 
			}
		}
	}
    if((ReceiveBuff1[3] == 'C')&&(ReceiveBuff1[4] == 'M')&&((ReceiveBuff1[5] == 'E')||(ReceiveBuff1[5] == 'S'))&&(ReceiveBuff1[8] == 'R'))
    {
        if((ReceiveBuff1[14] == '1')&&(ReceiveBuff1[15] == '0')&&(ReceiveBuff1[16] == 0x0D)){
            SimErrorFlag = 1;
            Modem_Tmr = 0;
            Modem_Cmd_Sent = 0; 
            Modem_Init_State = 0;
            memset(ReceiveBuff1,' ',350);
            ReceiveCounter1 = 0;
            Com2StringSend_with_LRC("STS002",6);           //  To Indicate No SIM Card Detected
        }
    }
    if(!strncmp(ReceiveBuff1+2,"RDY",3))
    {
        Modem_Tmr = 0;
        Modem_Cmd_Sent = 0; 
        Modem_Init_State = 0;
        memset(ReceiveBuff1,' ',350);
        ReceiveCounter1 = 0;
        GPRS_Flag = SMS_Flag = SMS_Response_Flag = 0;
        SignalLevelCheck = 0;
        GPRSTimer = 0;
        GPRS_Flags1.SignalOk = 0;
        SignalLevel = 0;
        SignalTimer = 0;
        Modem_Init_Cntr = 0;
		SignalLevelTimer = 0;    
    }
    if((ReceiveBuff1[3] == 'Q') && (ReceiveBuff1[5] == 'U')  && (ReceiveBuff1[8] == ':') &&
        (ReceiveBuff1[11] == 'p') && (ReceiveBuff1[13] == 'p') && (ReceiveBuff1[14] == 'd') && (ReceiveBuff1[16] == 'a'))
    {
        QIACT_Flag = GPRS_Flag = 0;
        SMS_Flag = 0;
        Com2StringSend("pdpdeact-done",13);
    }
    if((ReceiveBuff1[3] == 'Q') && (ReceiveBuff1[5] == 'U')  && (ReceiveBuff1[8] == ':') &&
        (ReceiveBuff1[11] == 'c') && (ReceiveBuff1[13] == 'o') && (ReceiveBuff1[14] == 's') && (ReceiveBuff1[16] == 'd'))
    {
        QIACT_Flag = GPRS_Flag = 0;
        SMS_Flag = 0;
        Com2StringSend("URC-closed   ",13);         //  Working
    }
}

void Data_Receiving(void)
{
unsigned char CheckSumFlag = 0;
char ProcessBuff[500] = {0};
unsigned int ProcessCnt = 0;
char DataBuff[16] = {0};
unsigned int TempBuff_Length,TempBuffNo = 0;
unsigned long TempBuff_Address = 0;
char CurrRespBuff[63] = {0};

    if(Flags.U1Ready == 1)
    {
        ProcessCnt = ReceiveCounter;
        memcpy(ProcessBuff,ReceiveBuff,ProcessCnt);
        LRC = CalcLRC(ProcessBuff,(ProcessCnt-2));							//	LRC Calculation
        memset(DataBuff,' ',16);
        memcpy(DataBuff,ProcessBuff+(ProcessCnt-2),2);
        if(LRC == atoi(DataBuff))                                               //	Comparing Calculated and Received LRC
            CheckSumFlag = 1;
        else
            CheckSumFlag = 0;
        
//#ifdef  TESTING
//		sprintf(DataBuff,"%d C:%d R:%d   ",ReceiveCounter,LRC,atoi(DataBuff));
//		LcdDisplay(TOP,DataBuff);
//        Delay1_1s(1);
//        //Testing();
//#endif
        if(CheckSumFlag == 1)
        {
            TempBuffNo = (ProcessBuff[0] - 48)-1;   //  Taking Bank Number // Normally in "char" Changing to ASCII value
            if((!strncmp(ProcessBuff+1,"CFG",3)) && (ProcessCnt == 15))           //  For Protocol [CFGBLLLAAAAAAXX] - 15bytes  - ([ and ] excluded))
            {                                                                         //             [BCFGLLLAAAAAAXX]
                memset(DataBuff,' ',16);memcpy(DataBuff,ProcessBuff+4,3);        //  Taking Buffer [1CFG14700050005]length 
                TempBuff_Length = atoi(DataBuff)-2;
                memset(DataBuff,' ',16);memcpy(DataBuff,ProcessBuff+7,6);        //  Taking Buffer Address
                TempBuff_Address = atol(DataBuff);
                
                if((TempBuff_Length > 500)||(TempBuff_Address < 500)){
                    Com2StringSend_with_LRC("ERR004",6);                        //  Error - Invalid length/Address    //
                    Flags.U1Ready = ReceiveCounter = ProcessCnt = 0;
                    memset(ReceiveBuff,' ',500);memset(ProcessBuff,' ',500);
                    return;
                }
                if(TempBuffNo <= 2){                                            //  Getting Details about buffers and storing
                    Buff[TempBuffNo].Length = TempBuff_Length;
                    Write_E2((EE_Buff1_Length+(2*(TempBuffNo))),(char*)&Buff[TempBuffNo].Length,sizeof(Buff[TempBuffNo].Length));
                    Read_E2((char*)&Buff[0].Length,EE_Buff1_Length,sizeof(Buff[0].Length));
                    Read_E2((char*)&Buff[1].Length,EE_Buff2_Length,sizeof(Buff[1].Length));
                    Read_E2((char*)&Buff[2].Length,EE_Buff3_Length,sizeof(Buff[2].Length));
                    
                    Buff[TempBuffNo].StartAddress = TempBuff_Address;
                    Write_E2((EE_Buff1_Address+(4*(TempBuffNo))),(char*)&Buff[TempBuffNo].StartAddress,sizeof(Buff[TempBuffNo].StartAddress));
                    Read_E2((char*)&Buff[0].StartAddress,EE_Buff1_Address,sizeof(Buff[0].StartAddress));
                    Read_E2((char*)&Buff[1].StartAddress,EE_Buff2_Address,sizeof(Buff[1].StartAddress));
                    Read_E2((char*)&Buff[2].StartAddress,EE_Buff3_Address,sizeof(Buff[2].StartAddress));

                    Rec[0].MaxRecLimit = ((Buff[1].StartAddress-1) - Buff[0].StartAddress)/Buff[0].Length;
                    Rec[1].MaxRecLimit = ((Buff[2].StartAddress-1) - Buff[1].StartAddress)/Buff[1].Length;
                    Rec[2].MaxRecLimit = (131072 - Buff[2].StartAddress)/Buff[2].Length;    //  128kB Memory
                    Write_E2(EE_Buff1_MaxRec,(char*)&Rec[0].MaxRecLimit,sizeof(Rec[0].MaxRecLimit));
                    Write_E2(EE_Buff2_MaxRec,(char*)&Rec[1].MaxRecLimit,sizeof(Rec[1].MaxRecLimit));
                    Write_E2(EE_Buff3_MaxRec,(char*)&Rec[2].MaxRecLimit,sizeof(Rec[2].MaxRecLimit));
                    Com2StringSend_with_LRC("CFG OK",6);
//#ifdef  TESTING
//        Testing();
//#endif
                }
                else{
                    CheckSumFlag = ProcessCnt = 0;
                    Com2StringSend_with_LRC("ERR003",6);    //  Bank Number Error in Bank Config Protocol   //
                    Flags.U1Ready = ReceiveCounter = ProcessCnt = 0;
                    memset(ReceiveBuff,' ',500);memset(ProcessBuff,' ',500);
                    return;                                 //  Once Error, then returned
                }
            }
            else if(((ProcessCnt == Buff[0].Length)&&(TempBuffNo == 0))||((ProcessCnt == Buff[1].Length)&&(TempBuffNo ==1))
                || ((ProcessCnt == Buff[2].Length)&&(TempBuffNo == 2)))
            {
                Data_Store(TempBuffNo,(ProcessBuff));
                if(((ProcessCnt == Buff[2].Length)&&(TempBuffNo == 2)))
                {
                    RTC.Month = (char)typecase_String_to_long(ProcessBuff+62,2);  //63 for bin monitoring
                    RTC.Date = (char)typecase_String_to_long(ProcessBuff+64,2);
                    RTC.Year = (int)typecase_String_to_long(ProcessBuff+66,4);

                    RTC.Hour = (unsigned char)typecase_String_to_long(ProcessBuff+69,2);
                    RTC.Minute = (char)typecase_String_to_long(ProcessBuff+71,2);
                    RTC.Second = (char)typecase_String_to_long(ProcessBuff+73,2);
                
                    if(Modem_setup.RTC_Date_Temp != RTC.Date)
                     {
                        
                         Modem_setup.Per_Day_SMS_Count = 0;
                         Modem_setup.Per_Day_Server_data_usage = 1;
                         Modem_setup.RTC_Date_Temp = RTC.Date;
                         Write_E2(EE_RTC_DATE_Temp,(char*)&Modem_setup.RTC_Date_Temp,sizeof(Modem_setup.RTC_Date_Temp));
                         Write_E2(EE_Per_Day_SMS_Count,(char*)&Modem_setup.Per_Day_SMS_Count,sizeof(Modem_setup.Per_Day_SMS_Count));
                         Write_E2(EE_Per_Day_Server_data_usage,(char*)&Modem_setup.Per_Day_Server_data_usage,sizeof(Modem_setup.Per_Day_Server_data_usage));
                         
                     } 
                    if(Modem_setup.RTC_Year_Temp != RTC.Year)
                     {
                         Modem_setup.SMS_Missing_count = 0;
                         Modem_setup.Missing_Server_data = 0;
                         Modem_setup.RTC_Year_Temp = RTC.Year;
                         Write_E2(EE_RTC_Year_Temp,(char*)&Modem_setup.RTC_Year_Temp,sizeof(Modem_setup.RTC_Year_Temp));
                         Write_E2(EE_Missing_Server_data,(char*)&Modem_setup.Missing_Server_data,sizeof(Modem_setup.Missing_Server_data));
                         Write_E2(EE_SMS_Missing_count,(char*)&Modem_setup.SMS_Missing_count,sizeof(Modem_setup.SMS_Missing_count));
                     
                    }
                }
               
            }
            else if(!strncmp(ProcessBuff,"LCSCOM",6))   
            {
//#ifdef  TESTING
//            Testing();
//#endif
            }
            else if((TempBuffNo == 8) && (ProcessCnt == Buff[2].Length)){       //  Immediate Data Buffer
                GPRSTimer = 0;
                GPRS_ResetFlag = 0;
                GPRS_Flag = 0;
                //SMS_Flag = 0;
                //SentDataCopied = 0;
                memcpy(ImmediateSentBuff1,ProcessBuff,Buff[2].Length);
                ImmediateSentFlag = 1;
                sprintf(CurrRespBuff,"BUF9OK*L%03d*S%02d*F9%02d*R1%04d*S1%04d*R2%04d*S2%04d*R3%04d*S3%04d*",SignalLevelCheck,Modem_Init_State,GPRS_Flag,Rec[0].DisplayNo,Sent[0].DisplayNo,Rec[1].DisplayNo,Sent[1].DisplayNo,Rec[2].DisplayNo,Sent[2].DisplayNo);
                Com2StringSend_with_LRC(CurrRespBuff,63);
                
                RTC.Month = (char)typecase_String_to_long(ProcessBuff+63,2);
                RTC.Date = (char)typecase_String_to_long(ProcessBuff+65,2);
                RTC.Year = (int)typecase_String_to_long(ProcessBuff+67,4);
                          
                RTC.Hour = (unsigned char)typecase_String_to_long(ProcessBuff+71,2);
                RTC.Minute = (char)typecase_String_to_long(ProcessBuff+73,2);
                RTC.Second = (char)typecase_String_to_long(ProcessBuff+75,2);
                
                if(Modem_setup.RTC_Date_Temp != RTC.Date)
                 {
                    
                     Modem_setup.Per_Day_SMS_Count = 0;
                     Modem_setup.Per_Day_Server_data_usage = 1;
                     Modem_setup.RTC_Date_Temp = RTC.Date;
                     Write_E2(EE_RTC_DATE_Temp,(char*)&Modem_setup.RTC_Date_Temp,sizeof(Modem_setup.RTC_Date_Temp));
                     Write_E2(EE_Per_Day_SMS_Count,(char*)&Modem_setup.Per_Day_SMS_Count,sizeof(Modem_setup.Per_Day_SMS_Count));
                     Write_E2(EE_Per_Day_Server_data_usage,(char*)&Modem_setup.Per_Day_Server_data_usage,sizeof(Modem_setup.Per_Day_Server_data_usage));
                     
                 }
                if(Modem_setup.RTC_Year_Temp != RTC.Year)
                     {
                         Modem_setup.SMS_Missing_count = 0;
                         Modem_setup.Missing_Server_data = 0;
                         Modem_setup.RTC_Year_Temp = RTC.Year;
                         Write_E2(EE_RTC_Year_Temp,(char*)&Modem_setup.RTC_Year_Temp,sizeof(Modem_setup.RTC_Year_Temp));
                         Write_E2(EE_Missing_Server_data,(char*)&Modem_setup.Missing_Server_data,sizeof(Modem_setup.Missing_Server_data));
                         Write_E2(EE_SMS_Missing_count,(char*)&Modem_setup.SMS_Missing_count,sizeof(Modem_setup.SMS_Missing_count));
                     }
            }
            else if(!strncmp(ProcessBuff,"RSTALL",6)){
                Bank_1_Data_Reset();
                Bank_2_Data_Reset();
                Bank_3_Data_Reset();
                SMS_GPRS_Data_Reset();
                Com2StringSend_with_LRC("RSTAOK",6);
                LcdDisplay(TOP,   "   All Memory   ");
                LcdDisplay(BOTTOM," Reset  Success ");
                Delay1_1s(1);
            }
            else if(!strncmp(ProcessBuff,"RST001",6)){
                Bank_1_Data_Reset();
                Com2StringSend_with_LRC("RST1OK",6);
                LcdDisplay(TOP,   "  Bank 1 Data   ");
                LcdDisplay(BOTTOM," Reset  Success ");
                Delay1_1s(1);
            }
            else if(!strncmp(ProcessBuff,"RST002",6)){
                Bank_2_Data_Reset();
                Com2StringSend_with_LRC("RST2OK",6);
                LcdDisplay(TOP,   "  Bank 2 Data   ");
                LcdDisplay(BOTTOM," Reset  Success ");
                Delay1_1s(1);
            }
            else if(!strncmp(ProcessBuff,"RST003",6)){
                Bank_3_Data_Reset();
                Com2StringSend_with_LRC("RST3OK",6);
                LcdDisplay(TOP,   "  Bank 3 Data   ");
                LcdDisplay(BOTTOM," Reset  Success ");
                Delay1_1s(1);
            }
            else if(!strncmp(ProcessBuff,"GPS ON",6)){
                GPS_Config = GPS_ON;
                Write_E2(EE_GPS_Config,(char*)&GPS_Config,sizeof(GPS_Config));
                Com2StringSend_with_LRC("GPONOK",6);
                LcdDisplay(TOP,   "     GPS ON     ");
                Delay1_1s(1);
            }
            else if(!strncmp(ProcessBuff,"GPSOFF",6)){
                GPS_Config = GPS_OFF;
                Write_E2(EE_GPS_Config,(char*)&GPS_Config,sizeof(GPS_Config));
                Com2StringSend_with_LRC("GPOFOK",6);
                LcdDisplay(TOP,   "     GPS OFF    ");
                Delay1_1s(1);
            }
            else if(!strncmp(ProcessBuff,"STATUS",6)){
                CurrentStatus();
            }
            else if(!strncmp(ProcessBuff,"TEST01",6)){
                GPS_Flag = 1;
            }
            else if(!strncmp(ProcessBuff,"TEST02",6)){
                GPS_Flag = 3;
            }
            else if(!strncmp(ProcessBuff,"LOADDF",6)){
                LoadDefault();
                Com2StringSend_with_LRC("LDDFOK",6);
            }
            else if(!strncmp(ProcessBuff,"SET",3)){
                memset(DataBuff,' ',16);memcpy(DataBuff,ProcessBuff+3,3);       //  Setting Mobile operator
                GPRS_Setup.MobileOptr = (char)atoi(DataBuff);
                Write_E2(EE_MobileOptr,(char*)&GPRS_Setup.MobileOptr,1);
                GPRS_ResetFlag = 1;
                Com2StringSend_with_LRC("SET OK",6);                            //  Mobile operator Selection Done    //
            }
            else if(!strncmp(ProcessBuff,"MWR",3)){
                if((ProcessBuff[3] == '*') && (ProcessBuff[6] == '*') && (ProcessBuff[10] == '*'))
                {
                    Store_Mob_Number(ProcessBuff,ProcessCnt);
                }
            }
            else if(!strncmp(ProcessBuff,"SSGRWR",6)){
                if((ProcessBuff[6] == '*') )
                {
                    Store_Set_SMS_GPRS_Retry(ProcessBuff,ProcessCnt);
                }
                
            }
            else if(!strncmp(ProcessBuff,"SSCWR",5)){
                if((ProcessBuff[5] == '*') )
                {
                    Store_Set_SMS_Count(ProcessBuff,ProcessCnt);
                }
                
            }
            
             else if(!strncmp(ProcessBuff,"SGLWR",5)){
                if((ProcessBuff[5] == '*') )
                {
                    Store_Set_GPRS_LIMIT(ProcessBuff,ProcessCnt);
                }
                
            }
           
            else if((!strncmp(ProcessBuff,"MRD",3))&&(ProcessCnt == 9)){
                if((ProcessBuff[3] == '*') && (ProcessBuff[6] == '*'))
                {
                    memset(DataBuff,' ',16);memcpy(DataBuff,ProcessBuff+4,2);
                    Read_Mob_Number(atoi(DataBuff)-1);
                }
            }
            else if(!strncmp(ProcessBuff,"SSGRRD",6)){                
                   if(ProcessBuff[6] == '*')
                   Read_Set_SMS_GPRS_Retry();                
            }
            else if(!strncmp(ProcessBuff,"SSCRD",5)){               
                  if(ProcessBuff[5] == '*')
                   Read_Set_SMS_Count();                
            }
            else if(!strncmp(ProcessBuff,"SGLRD",5)){               
                 
                if(ProcessBuff[5] == '*')
                Read_Set_GPRS_LIMIT();                
            }
            else if(!strncmp(ProcessBuff,"VSCRD",5)){               
                  
                if(ProcessBuff[5] == '*')   
                Read_View_SMS_Count();                
            }
            else if(!strncmp(ProcessBuff,"VGLRD",5)){               
                  
                  if(ProcessBuff[5] == '*')
                   Read_View_GPRS_LIMIT();                
            }
            else
            {
                Com2StringSend_with_LRC("ERR002",6);
            }
        }
        else
        {
            Com2StringSend_with_LRC("ERR001",6);    //  Invalid LRC - Checksum Error    //
        }
        Flags.U1Ready = ReceiveCounter = ProcessCnt = 0;
        memset(ReceiveBuff,' ',500);memset(ProcessBuff,' ',500);
    }

}

void ReadSettings()
{
    Read_E2((char*)&GPRS_Setup.MobileOptr,EE_MobileOptr,1);Delay(2);

    Read_E2((char*)&Buff[0].Length,EE_Buff1_Length,sizeof(Buff[0].Length));
    Read_E2((char*)&Buff[1].Length,EE_Buff2_Length,sizeof(Buff[1].Length));
    Read_E2((char*)&Buff[2].Length,EE_Buff3_Length,sizeof(Buff[2].Length));
    Read_E2((char*)&Buff[0].StartAddress,EE_Buff1_Address,sizeof(Buff[0].StartAddress));
    Read_E2((char*)&Buff[1].StartAddress,EE_Buff2_Address,sizeof(Buff[1].StartAddress));
    Read_E2((char*)&Buff[2].StartAddress,EE_Buff3_Address,sizeof(Buff[2].StartAddress));

    Read_E2((char*)&Rec[0].MaxRecLimit,EE_Buff1_MaxRec,sizeof(Rec[0].MaxRecLimit));
    Read_E2((char*)&Rec[1].MaxRecLimit,EE_Buff2_MaxRec,sizeof(Rec[1].MaxRecLimit));
    Read_E2((char*)&Rec[2].MaxRecLimit,EE_Buff3_MaxRec,sizeof(Rec[2].MaxRecLimit));

    Read_E2((char*)&Rec[0].Avail,EE_Buff1_RecAvail,sizeof(Rec[0].Avail));
    Read_E2((char*)&Rec[1].Avail,EE_Buff2_RecAvail,sizeof(Rec[1].Avail));
    Read_E2((char*)&Rec[2].Avail,EE_Buff3_RecAvail,sizeof(Rec[2].Avail));

    Read_E2((char*)&Sent[0].Avail,EE_Buff1_SentAvail,sizeof(Sent[0].Avail));
    Read_E2((char*)&Sent[1].Avail,EE_Buff2_SentAvail,sizeof(Sent[1].Avail));
    Read_E2((char*)&Sent[2].Avail,EE_Buff3_SentAvail,sizeof(Sent[2].Avail));

    Read_E2((char*)&Rec[0].DisplayNo,EE_Buff1_RecDisplayNo,sizeof(Rec[0].DisplayNo));
    Read_E2((char*)&Rec[1].DisplayNo,EE_Buff2_RecDisplayNo,sizeof(Rec[1].DisplayNo));
    Read_E2((char*)&Rec[2].DisplayNo,EE_Buff3_RecDisplayNo,sizeof(Rec[2].DisplayNo));

    Read_E2((char*)&Sent[0].DisplayNo,EE_Buff1_SentDisplayNo,sizeof(Sent[0].DisplayNo));
    Read_E2((char*)&Sent[1].DisplayNo,EE_Buff2_SentDisplayNo,sizeof(Sent[1].DisplayNo));
    Read_E2((char*)&Sent[2].DisplayNo,EE_Buff3_SentDisplayNo,sizeof(Sent[2].DisplayNo));

    Read_E2((char*)&Rec[0].InitFlag,EE_Buff1_InitFlag,sizeof(Rec[0].InitFlag));
    Read_E2((char*)&Rec[1].InitFlag,EE_Buff2_InitFlag,sizeof(Rec[1].InitFlag));
    Read_E2((char*)&Rec[2].InitFlag,EE_Buff3_InitFlag,sizeof(Rec[2].InitFlag));
    
    Read_E2((char*)&GPS_Config,EE_GPS_Config,sizeof(GPS_Config));
    
    Read_E2((char*)&Modem_setup.Retry_count,EE_Retry_Count,sizeof(Modem_setup.Retry_count));
    Read_E2((char*)&Modem_setup.Limite_SMS,EE_Limite_SMS,sizeof(Modem_setup.Limite_SMS));
    Read_E2((char*)&Modem_setup.Limite_Server_data_usage,EE_Limite_Server_data_usage,sizeof(Modem_setup.Limite_Server_data_usage));
    
    Read_E2((char*)&Modem_setup.Per_Day_SMS_Count,EE_Per_Day_SMS_Count,sizeof(Modem_setup.Per_Day_SMS_Count));
    Read_E2((char*)&Modem_setup.Per_Day_Server_data_usage,EE_Per_Day_Server_data_usage,sizeof(Modem_setup.Per_Day_Server_data_usage));
    Read_E2((char*)&Modem_setup.RTC_Date_Temp,EE_RTC_DATE_Temp,sizeof(Modem_setup.RTC_Date_Temp));
    Read_E2((char*)&Modem_setup.SMS_Missing_count,EE_SMS_Missing_count,sizeof(Modem_setup.SMS_Missing_count));
    Read_E2((char*)&Modem_setup.Missing_Server_data,EE_Missing_Server_data,sizeof(Modem_setup.Missing_Server_data));
    Read_E2((char*)&Modem_setup.RTC_Year_Temp,EE_RTC_Year_Temp,sizeof(Modem_setup.RTC_Year_Temp));
 
//    Buff[0].Length = 98;
//    Buff[1].Length = 98;
//    Buff[2].Length = 98;
//	Buff[0].StartAddress = 500;
//    Buff[1].StartAddress = 44000;
//    Buff[2].StartAddress = 87500;
//    Rec[0].MaxRecLimit = 440;
//    Rec[1].MaxRecLimit = 440;
//    Rec[2].MaxRecLimit = 440;
}

void Delay(unsigned long int counter)
{
	while(counter > 0) 
	{
		counter--;
	}
}

void PutDp(unsigned char*Tmp,unsigned long deci,char x,char displaymode)
{
	char i;
	unsigned char buff[20];

	memcpy(buff,Tmp,(int)x);

	if(deci != 0)
	{
		memmove((buff+(x-(char)deci+1)),(buff+(x-(char)deci)),(int)deci);
		*(buff+(x-(char)deci)) = '.';
	}
	else 
		*(buff+x)=' ';

	if(displaymode == 1)
	{
		for(i=0;i<(x-deci-1);i++)
		{
			if(*(buff+i) == '0')
				*(buff+i) = ' ';
			else if(*(buff+i) != '-') 
				break;
		}
	}
	if(deci != 0)
		memcpy(Tmp,buff,x+1);
	else
	{
		memcpy((Tmp+1),buff,x);
		*Tmp=' ';
	}
	return;
}

void PowerOn()
{
	Initialise_Ports();
	Init_Oscillator();
	Init_Interrupt();
	ADC_Init();
	InitLCDDisplay();
	SRbits.IPL	= 0;
	UART_Config();
    GPRS_HW_RESET = 1;
}

void Initialise_Ports()
{
// port b-2,3,4,5 as output, rest as inputs 
//	TRISB = 0Xfffb;

//	TRISB = 0X3f0f;	//0011 1111 0000 1111	For matrix keypad Port b-4,5 as output and b-2,3 as input
	TRISB = 0X3f3f;	//0011 1111 0011 1111	For normal keypad Port b-2,3,4,5 as input	//sat


//	TRISB = 0x3fc3;	//0011 1111 1100 0011 For Matrix Keypad RB12,RB13 used as input RB14,RB15 used as output 
	PORTB=0x00;

//port G-15,14,13,12,11,10 input, 9,8,7,6 as output, 5,4 as input, 3,2 as output, 1 & 0 as input
//	TRISG = 0xfc33; 	 
	TRISG = 0xac33; 	 
	TRISC = 0x0006;		//PortC.1,2 as i/p
//	TRISF = 0xFFFF;
//	TRISD = 0xFFFF;

	TRISD = 0xF800;		// EWP_A RD8,EWP_B RD9 as o/p's
	
//	c0x03c3;//Nop();
//	PORTB=0x3c ;//Nop();
	
	PORTG=0x00;
}

void Init_Interrupt(void)
{
    
//	IPC0	= 0X0004;
/*
//	SR		= 0X0010;
	CORCON	= 0X0000;
//	INTCON1	= 0XE700;
	INTCON2	= 0X0000;
//	IFS0	= 0X0201;
	IFS1bits.C1IF	= 1;
	IFS1bits.C1IF	= 1;

//	IFS2	= 0X0000;
//	IEC0	= 0X1201;
	IEC1bits.C1IE   = 1;
//	IEC1	= 0X0800;

//	IEC2	= 0X0000;

//	IPC0	= 0X0004;
//	IPC1	= 0X0000;
//	IPC2	= 0X0000;
//	IPC3	= 0X0000;
//	IPC4	= 0X0000;
//	IPC5	= 0X0000;
//	IPC6	= 0X0004;
//	IPC7	= 0X0000;
//	IPC8	= 0X0000;
//	IPC9	= 0X0000;
//	IPC10	= 0X0000;
//	INTTREG	= 0X0200;	//0010 0000 0000
	
IPC7	= 0X0000;
*/	
	

	INTCON2 = 0x001E;       /*Setup INT1, INT2, INT3 & INT4 pins to interupt */
                                /*on falling edge and set up INT0 pin to interupt */
                                /*on rising edge */

//      IFS0bits.INT0IF = 0;    /*Reset INT0 interrupt flag */
//      IEC0bits.INT0IE = 1;    /*Enable INT0 Interrupt Service Routine */

//      IFS1bits.INT1IF = 0;    /*Reset INT1 interrupt flag */
//	  IEC1bits.INT1IE = 1;    /*Enable INT1 Interrupt Service Routine */

//      IFS1bits.INT2IF = 0;    /*Reset INT2 interrupt flag */
//      IEC1bits.INT2IE = 1;    /*Enable INT2 Interrupt Service Routine */

//      IFS2bits.INT3IF = 0;    /*Reset INT3 interrupt flag */
//      IEC2bits.INT3IE = 1;    /*Enable INT3 Interrupt Service Routine */

      IFS2bits.INT4IF = 0;    /*Reset INT4 interrupt flag */
      IEC2bits.INT4IE = 1;    /*Enable INT4 Interrupt Service Routine */


	/* ensure Timer 1 is in reset state */
	T1CON = 0;
	/* Reset Timer1 interrupt flag */
	IFS0bits.T1IF = 0;
	T1CONbits.TSIDL=0;		// Resume even if in idle mode
 	PR1	  =	Delay_1mS_Cnt;	//  Value = ( 1 / (FCY / pres)) * PR1) = delay 0x1D4C
 	/* select external timer clock */
	T1CONbits.TCS = 0;
	T1CONbits.TGATE=0;
	T1CONbits.TSYNC=0;
	IEC0bits.T1IE = 1;  	// Enable Timer1 interrupt 
	T1CONbits.TON = 1;		/* enable Timer 1 and start the count */ 
	IPC0	&= ~(0x000F<<12);
	IPC0	|= 0x3000;
}

void Com1Init()
{
	/* COM PORT 1*/
	U1MODE = 0xA080;
	U1STAbits.UTXISEL = 0;
	U1STAbits.UTXBRK  = 0;
	U1STAbits.UTXEN	  = 1;
	U1STAbits.URXISEL = 0;
	U1STAbits.ADDEN	  = 1;
	U1STAbits.URXISEL = 1;      // enable receive interrupt when a character is available
	U1STAbits.UTXISEL = 1;      // enable transmit interrupt when a character is available

	U1BRG  = Baud115200;        //  Baud115200 = for EC25-E default Baud Rate
	IEC0bits.U1RXIE   	= 1;	//  Receiver Interrupt Enable bit		
	IEC0bits.U1TXIE   	= 1;	//  Transmit Interrupt Enable bit	
	
	IFS0bits.U1TXIF = 0;		// clear TX interrupt flag
	IFS0bits.U1RXIF = 0;		// clear RX interrupt flag
	U1STAbits.OERR = 0;         // Clear Overrun Error to receive data
	U1STAbits.FERR =0;
	U1STAbits.PERR =0;
	_TRISF2=1;_TRISF3=0;
	_RF2=0;_LATF3=0;
	IPC2	&= ~(0x000F<<8);	//UART-1 TX has the lowest priority
	IPC2	|= 0x0100;	//UART-1 TX has the lowest '1' priority
	IPC2	&= ~(0x000F<<4);	// Receiver has priority as '2' 
	IPC2	|= 0x0020;	//UART-1 RX has the next '2' priority
 	Flags.Stx1Received=0;Flags.Etx1Received=0;
	CharIn1=0;CharOut1=0;BytesToSend1=0;
}	

void Com2Init()
{
	/* COM PORT 2*/
	U2MODE = 0xA080;
	U2STAbits.UTXISEL = 0;
	U2STAbits.UTXBRK  = 0;
	U2STAbits.UTXEN	  = 1;
	U2STAbits.URXISEL = 0;
	U2STAbits.ADDEN	  = 1;
	U2STAbits.URXISEL = 1;	// enable interrupt when a character is available
	U2STAbits.UTXISEL = 1;	// enable transmit interrupt when a character is available
	
	U2BRG  = Baud9600;	
	IEC1bits.U2RXIE   	= 1;	//Receiver Interrupt Enable bit		
	IEC1bits.U2TXIE   	= 1;	//transmit Interrupt Enable bit		

	IFS1bits.U2TXIF = 0;		 // clear TX interrupt flag
	IFS1bits.U2RXIF = 0;		 // clear RX interrupt flag
	U2STAbits.OERR = 0; 		// Clear Overrun Error to receive data
	U2STAbits.FERR =0;
	U2STAbits.PERR =0;
	_TRISF4=1;_TRISF5=0;
	_RF4=0;_LATF5=0;

	IPC6	&= ~(0x000F<<4);	//UART-2 TX has the lowest priority
	IPC6	|= 0x0060;	//UART-2 TX has the lowest '1' priority
	IPC6	&= ~(0x000F<<0);	// Receiver has priority as '2' 
	IPC6	|= 0x0007;	//UART-2 RX has the next '2' priority

 	Flags.Stx2Received=0;Flags.Etx2Received=0;
 	CharIn2=0;CharOut2=0;BytesToSend2=0;
}	

void UART_Config()
{
	Com1Init();
	Com2Init();	
}


void Delay_ms(unsigned char Delay)
{
	Delayval=Delay;
	while (Delayval !=0);
	return ;
}

void Com1Bytesend(char c)		/*  COM 1 byte send*/
{
	U1TXREG = c;		
	while(!U1STAbits.TRMT);
}

void Com1StringSend(char * Buff,int NoOfBytes)	/*  COM 2 string send*/
{
	int Increment;

	for(Increment=0; Increment < NoOfBytes; Increment++){	// This loop reads in the text string and 
		Com1Bytesend(Buff[Increment]);		
	}
}

void Com2Bytesend(char c)	/*  COM 2 byte send*/
{
	U2TXREG = c;		
	while(!U2STAbits.TRMT);
}

void Com2StringSend(char * Buff,int NoOfBytes)	/*  COM 2 string send*/
{
	int Increment;

	for(Increment=0; Increment < NoOfBytes; Increment++){	// This loop reads in the text string and 
		Com2Bytesend(Buff[Increment]);		
	}
}

void _ISR __attribute__((interrupt)) _T1Interrupt(void)
{	
	if(One_Second_Counter++ > 1500){
        One_Second_Counter = 0;
        DisplayLocation++;
        if(DisplayLocation > 2)
        {
            DisplayLocation = 0;
        }
    }
    
    if(GPS_Config == GPS_ON)
    {
        if(GPS_Getting_Timer++ > 1000)
        {
            GPS_Getting_Timer = 0;
            GPS_Getting_Sec++;
        }
    }
	
	if (Delayval > 0 )
	 	Delayval--;

	if(TimeOutFlag.TimeOutEnb == 1)			// Fot TimeOut Checking during password entry 
	{
		if(TimeOut_Cnt++ >= TIMEOUT)
		{
			TimeOutFlag.TimeOut = 1;
			TimeOut_Cnt = 0;	
		}
	}
    
    if(Modem_Init_State == MODEM_OK)
    {
        if((GPRS_Flag != 0) && (GPRS_Flag < 12))
            GPRSTimer++;  

        
        if((SMS_Flag != 0) && (SMS_Flag < 8))
            SMS_Timer++;
        else
            SMS_Timer = 0;
        
    }
    
   if((((GPRS_Flag == 0)||(SMS_Flag == 0)) && (Modem_Init_State == MODEM_OK)) || (GPRS_Flags1.Modem_Rdy == 1))
       Alivecounter++;
    else
       Alivecounter = 0;
    
    IFS0bits.T1IF = 0;
}
void _ISR __attribute__((interrupt)) _U1RXInterrupt(void)
{
	if(U1STAbits.OERR == 1) {
		U1STAbits.OERR = 0; // Clear Overrun Error to receive data
	} 
//	if ((U1STAbits.FERR ==0) && (U1STAbits.PERR ==0) ) 
	{
		ReceivedChar1 = U1RXREG; // Read Data if there is no parity or framing error
        
        ReceiveBuff2[ReceiveCounter2++] = ReceivedChar1;  //Sivaprakash
		if(ReceiveCounter2 >= 1350)
			ReceiveCounter2 = 0;
        
        
		if(ReceivedChar1 == 'U')
			Bits.Com1Check = 1;	

        if(ReceivedChar1 == 'C') 
            Modem_Response_Flag = 1;
        else if((ReceivedChar1 == ':') && (Modem_Response_Flag == 1))
            Modem_Response_Flag = 2;
        else if((ReceivedChar1 == ' ') && (Modem_Response_Flag == 2))
            Modem_Response_Flag = 3;
        else if((ReceivedChar1 == '"') && (Modem_Response_Flag == 3))
            Modem_Response_Flag = 4;
        else if(((ReceivedChar1 == 'c')||(ReceivedChar1 == 'p')) && (Modem_Response_Flag == 4))
            Modem_Response_Flag = 5;
        else if(((ReceivedChar1 == 'l')||(ReceivedChar1 == 'd')) && (Modem_Response_Flag == 5))
            Modem_Response_Flag = 6;
        else if(((ReceivedChar1 == 'o')||(ReceivedChar1 == 'p')) && (Modem_Response_Flag == 6))
            Modem_Response_Flag = 7;
        else if(((ReceivedChar1 == 's')||(ReceivedChar1 == 'd')) && (Modem_Response_Flag == 7))
            Modem_Response_Flag = 8;
        else if(((ReceivedChar1 == 'e')||(ReceivedChar1 == 'e')) && (Modem_Response_Flag == 8))
            Modem_Response_Flag = 9;
        else if(((ReceivedChar1 == 'd')||(ReceivedChar1 == 'a')) && (Modem_Response_Flag == 8))
            Modem_Response_Flag = 9;
        else
            Modem_Response_Flag = 0;
        
        
        
		if(GPRS_Flag == 9)
		{
            if((ReceivedChar1 == ']') && (Flags.U2Start == 1))
            {
                Flags.U2Start = 0;
                GPRS_Uart1Flag = 1;
            }
            else if(Flags.U2Start == 1)
            {
                ReceiveBuff1[ReceiveCounter1] = ReceivedChar1;			
                ReceiveCounter1++;
            }
            else if((ReceivedChar1 == '[')&&(Flags.U2Start == 0))
            {
                ReceiveCounter1 = 0;
                GPRS_Uart1Flag = 0;
                Flags.U2Start = 1;
            }
		}
        else 
            if(SMS_Flag == 7)
        {
            if(ReceivedChar1 == 'G') 
                SMS_Response_Flag = 1;
            else if((ReceivedChar1 == 'S') && (SMS_Response_Flag == 1))
                SMS_Response_Flag = 2;
            else if((ReceivedChar1 == ':') && (SMS_Response_Flag == 2))
            {
                SMS_Response_Flag = 3;
            }
        }
		else
			ReceiveBuff1[ReceiveCounter1++] = ReceivedChar1;
		if(ReceiveCounter1 >= 350)
			ReceiveCounter1 = 0;	

	}
	IFS0bits.U1RXIF = 0; // clear RX interrupt flag}
}

void _ISR __attribute__((interrupt)) _U1TXInterrupt(void)
{
	Flags.SendData1 = True ;

	if (BytesToSend1!=0) {
		if (BytesSent1 < BytesToSend1) {
			U1TXREG = PackToSend1[BytesSent1];
			BytesSent1++;
		}
		else 
			{BytesToSend1=0;}
	}
	IFS0bits.U1TXIF = 0; // clear RX interrupt flag}
}

void _ISR __attribute__((interrupt)) _U2RXInterrupt(void)
{
	if(U2STAbits.OERR == 1) {
		U2STAbits.OERR = 0; // Clear Overrun Error to receive data
	} 
	ReceivedChar2 = U2RXREG;	// Read Data if there is no parity or framing error

    if(ReceivedChar2 == 'U')
        Bits.Com2Check = 1;	

    if(Flags.U1Ready == 0)
    {
        if((ReceivedChar2 == ']') && (Flags.U1Start == 1))          //      if((Bits.UART_2_Start == 1)&& (ReceivedChar2 == '$'))
        {
            Flags.U1Start = 0;
            Flags.U1Ready = 1;
        }
        else if(Flags.U1Start == 1)
        {
            ReceiveBuff[ReceiveCounter] = ReceivedChar2;			
            ReceiveCounter++;
        }
        else if((ReceivedChar2 == '[')&&(Flags.U1Start == 0))
        {
            ReceiveCounter = 0;
            Flags.U1Ready = 0;
            Flags.U1Start = 1;
        }
    }
IFS1bits.U2RXIF = 0; // clear RX interrupt flag
}

void _ISR __attribute__((interrupt)) _U2TXInterrupt(void)
{
	Flags.SendData2 = True ;

	if (BytesToSend2!=0) {
		if (BytesSent2 < BytesToSend2) {
			U2TXREG = PackToSend2[BytesSent2];
			BytesSent2++;
		}
		else 
			{BytesToSend2=0;}
	}
	IFS1bits.U2TXIF = 0; // clear RX interrupt flag}
}


void __attribute__((__interrupt__)) _ADCInterrupt(void)
{
		Flags.ADDATAREADY  = 1;
		ADCOUNTS = ADCBUF0;
        IFS0bits.ADIF = 0;
//		 if(ADCntr++ >= 999999)
//			ADCntr = 0;
}

void __attribute__((__interrupt__)) _INT0Interrupt(void)
{

	if(IFS0bits.INT0IF)
	{	
		IFS0bits.INT0IF = 0;
	}
		    //Clear the INT0 interrupt flag or else
            //the CPU will keep vectoring back to the ISR
}

void __attribute__((__interrupt__)) _INT1Interrupt(void)	// interrupt one 
{
	if(IFS1bits.INT1IF)	// interrupt one status flag
	{	
//		InterruptFlag++;
	//	EEPROMINC_VAL++;
		IFS1bits.INT1IF = 0;		
	}
		
}
void __attribute__((__interrupt__)) _INT2Interrupt(void)		// interrupt two
{
	if(IFS1bits.INT2IF)		// interrupt two  status flag
	{	
//		InterruptFlag++;
	//	EEPROMINC_VAL++;
		IFS1bits.INT2IF = 0;		
	}
		
}
void __attribute__((__interrupt__)) _INT3Interrupt(void)		// interrupt three 
{
	if(IFS2bits.INT3IF)		// interrupt three status flag
	{	
//		InterruptFlag++;
	//	EEPROMINC_VAL++;
		IFS2bits.INT3IF = 0;		
	}
		
}

void __attribute__((__interrupt__)) _INT4Interrupt(void)		// interrupt four
{
	if(IFS2bits.INT4IF)		// interrupt four status flag
	{	
//		PulseCount++;
		IFS2bits.INT4IF = 0;		
	}
		
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
		_TRISB12 = 1;		// Input Config
		_TRISB13 = 1;		// Input Config

		//ADPCFG Register
        //Set up channels AN8,9,10,11,12,13 as analog input
        ADPCFG = 0xFFFF;

        _PCFG8  = 0;
		_PCFG9  = 0;
		_PCFG10 = 0;
		_PCFG11 = 0;	
		_PCFG12 = 0;
		_PCFG13 = 0;	

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

		Flags.ADDATAREADY = 0;
		while(!Flags.ADDATAREADY);
	
		for (count = 0; count < 16; count++) // average the 15 ADC value
		{
		  ADCValue = ADCValue + *ADC16Ptr++;
		}

		ADCValue = ADCValue >> 4;		//16 pgm samples 
		ADCSampleVal+= ADCValue;
	}

 	ADCSampleVal  = ADCSampleVal / 20;
    ADCSampleVal  = ADCSampleVal >> 1;
}

void PortCheck(char Port)
{
	Bits.Com1Check = Bits.Com2Check = 0;
	while(1)
	{
		if(Port == 1)
			Com1Bytesend('U');
		else
			Com2Bytesend('U');

		KeySense();
		if(MenuKey)
		{
			break;
		}

		if(Bits.Com1Check || Bits.Com2Check)
		{
			Bits.Com1Check = 0;
			Bits.Com2Check = 0;
			memcpy(LcdDispBuff+16,"  Status: PASS  ",16);
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			Delay1_1s(1);
		}
		else
		{
			memcpy(LcdDispBuff+16,"  Status: FAIL  ",16);	
			LcdDisplay(BOTTOM,LcdDispBuff+16);
		}
		
		if(TimeOutFlag.TimeOut)
			return;
	}
	memset(LcdDispBuff+16,' ',16);
	
	return;
}


unsigned char GetLSB(unsigned int Integer)
{
	return (Integer & 0xff);
}

unsigned char GetMSB(unsigned int Integer)
{
	return ((Integer >> 8 ) & 0x0ff);
}

void SelMobileOperator()
{
	unsigned char i = 0;
	i = GPRS_Setup.MobileOptr;
	while(1)
	{
		if(i == 0)
			LcdDisplay(BOTTOM,"     AIRTEL     ");
		else if(i == 1)
			LcdDisplay(BOTTOM,"   TATA DOCOMO  ");
		else if(i == 2)
			LcdDisplay(BOTTOM,"      IDEA      ");
		else if(i == 3)
			LcdDisplay(BOTTOM,"     AIRCEL     ");
		else if(i == 4)
			LcdDisplay(BOTTOM,"      BSNL      ");

		KeySense();
		if(EnterKey)
		{
			//if(SetOperator(i))
			{
			 GPRS_Setup.MobileOptr = i;
				LcdDisplay(BOTTOM,"     STORED     ");
				Write_E2(EE_MobileOptr,(char*)&GPRS_Setup.MobileOptr,1);
				break;
			}
			//else
			//{
			//	LcdDisplay(BOTTOM,"     ERROR      ");
			//	break;
			//}
		}		
		else if(UpKey)
		{
			if(i++ >= 4)
				i = 0;
		}
		else if(EscKey)
		{
			break;
		}

	}
}

char SetOperator(char i)
{
	unsigned int Timer = 0;
	Com1StringSend("At+QIREGAPP=",12);
	if(i == 0)
		Com1StringSend("\"airtelgprs.com\"",16);	
	else if(i == 1)
		Com1StringSend("\"www\"",5);
	else if(i == 2)
		Com1StringSend("\"TATA.DOCOMO.INTERNET\"",22);
	else if(i == 3)
		Com1StringSend("\"aircelgprs.pr\"",15);
	else if(i == 4)
		Com1StringSend("\"imis/internet\"",15);
	else if(i == 5)
		Com1StringSend("\"bsnlnet\"",9);
	else if(i == 6)
		Com1StringSend("\"rcomnet\"",9);
	else if(i == 7)
		Com1StringSend("\"uninor\"",8);
	Com1StringSend(",\" \",\" \"",8);
	Com1Bytesend(0x0d);
	Com1Bytesend(0x0a);
	ReceiveCounter1 = 0;memset(ReceiveBuff1,' ',350);
	while(1)
	{
		if(ReceiveBuff1[2] == 'O' && ReceiveBuff1[3] == 'K')
			return(1);
		else if(ReceiveBuff1[2] == 'E' && ReceiveBuff1[6] == 'R')
			return(0);	
	 	if(Timer++ > 25000)
			break;
	}
	return(0);
}

void LoadDefault()
{
    char WR_LD_Mob_Number[15] = {"               "};
    int Mob_Cnt = 0;
    
	MemoryCheck = 71; 
    Write_E2(EE_MemoryCheck,(char*)&MemoryCheck,sizeof(MemoryCheck));Delay(10);

	GPRS_Setup.MobileOptr = 1;
	Write_E2(EE_MobileOptr,(char*)&GPRS_Setup.MobileOptr,1);

    Buff[0].Length = 123;
    Buff[1].Length = 98;
    Buff[2].Length = 98;
    Write_E2(EE_Buff1_Length,(char*)&Buff[0].Length,sizeof(Buff[0].Length));Delay(2);
    Write_E2(EE_Buff2_Length,(char*)&Buff[1].Length,sizeof(Buff[1].Length));Delay(2);
    Write_E2(EE_Buff3_Length,(char*)&Buff[2].Length,sizeof(Buff[2].Length));Delay(2);
    
    Buff[0].StartAddress = 500;
    Buff[1].StartAddress = 44000;
    Buff[2].StartAddress = 87500;
    Write_E2(EE_Buff1_Address,(char*)&Buff[0].StartAddress,sizeof(Buff[0].StartAddress));Delay(2);
    Write_E2(EE_Buff2_Address,(char*)&Buff[1].StartAddress,sizeof(Buff[1].StartAddress));Delay(2);
    Write_E2(EE_Buff3_Address,(char*)&Buff[2].StartAddress,sizeof(Buff[2].StartAddress));Delay(2);
    
    Rec[0].MaxRecLimit = ((Buff[1].StartAddress-1) - Buff[0].StartAddress)/Buff[0].Length;
    Rec[1].MaxRecLimit = ((Buff[2].StartAddress-1) - Buff[1].StartAddress)/Buff[1].Length;
    Rec[2].MaxRecLimit = (131072 - Buff[2].StartAddress)/Buff[2].Length;    //  128kB Memory
    Write_E2(EE_Buff1_MaxRec,(char*)&Rec[0].MaxRecLimit,sizeof(Rec[0].MaxRecLimit));
    Write_E2(EE_Buff2_MaxRec,(char*)&Rec[1].MaxRecLimit,sizeof(Rec[1].MaxRecLimit));
    Write_E2(EE_Buff3_MaxRec,(char*)&Rec[2].MaxRecLimit,sizeof(Rec[2].MaxRecLimit));

    Rec[0].Avail = 0;
    Rec[1].Avail = 0;
    Rec[2].Avail = 0;
    Write_E2(EE_Buff1_RecAvail,(char*)&Rec[0].Avail,sizeof(Rec[0].Avail));Delay(2);
    Write_E2(EE_Buff2_RecAvail,(char*)&Rec[1].Avail,sizeof(Rec[1].Avail));Delay(2);
    Write_E2(EE_Buff3_RecAvail,(char*)&Rec[2].Avail,sizeof(Rec[2].Avail));Delay(2);
    
    Sent[0].Avail = 0;
    Sent[1].Avail = 0;
    Sent[2].Avail = 0;
    Write_E2(EE_Buff1_SentAvail,(char*)&Sent[0].Avail,sizeof(Sent[0].Avail));Delay(2);
    Write_E2(EE_Buff2_SentAvail,(char*)&Sent[1].Avail,sizeof(Sent[1].Avail));Delay(2);
    Write_E2(EE_Buff3_SentAvail,(char*)&Sent[2].Avail,sizeof(Sent[2].Avail));Delay(2);

    Rec[0].DisplayNo = 0;
    Rec[1].DisplayNo = 0;
    Rec[2].DisplayNo = 0;
    Write_E2(EE_Buff1_RecDisplayNo,(char*)&Rec[0].DisplayNo,sizeof(Rec[0].DisplayNo));Delay(2);
    Write_E2(EE_Buff2_RecDisplayNo,(char*)&Rec[1].DisplayNo,sizeof(Rec[1].DisplayNo));Delay(2);
    Write_E2(EE_Buff3_RecDisplayNo,(char*)&Rec[2].DisplayNo,sizeof(Rec[2].DisplayNo));Delay(2);
    
    Sent[0].DisplayNo = 0;
    Sent[1].DisplayNo = 0;
    Sent[2].DisplayNo = 0;
    Write_E2(EE_Buff1_SentDisplayNo,(char*)&Sent[0].DisplayNo,sizeof(Sent[0].DisplayNo));Delay(2);
    Write_E2(EE_Buff2_SentDisplayNo,(char*)&Sent[1].DisplayNo,sizeof(Sent[1].DisplayNo));Delay(2);
    Write_E2(EE_Buff3_SentDisplayNo,(char*)&Sent[2].DisplayNo,sizeof(Sent[2].DisplayNo));Delay(2);
    
    Rec[0].InitFlag = 0;
    Rec[1].InitFlag = 0;
    Rec[2].InitFlag = 0;
    Write_E2(EE_Buff1_InitFlag,(char*)&Rec[0].InitFlag,sizeof(Rec[0].InitFlag));Delay(2);
    Write_E2(EE_Buff2_InitFlag,(char*)&Rec[1].InitFlag,sizeof(Rec[1].InitFlag));Delay(2);
    Write_E2(EE_Buff3_InitFlag,(char*)&Rec[2].InitFlag,sizeof(Rec[2].InitFlag));Delay(2);
    Rec[0].OverFlowFlag = 0;
    Rec[1].OverFlowFlag = 0;
    Rec[2].OverFlowFlag = 0;
    
    for(Mob_Cnt = 0;Mob_Cnt<15;Mob_Cnt++)
    {
        Write_E2(EE_SMS_Number_1+(Mob_Cnt*15),(char*)&WR_LD_Mob_Number,sizeof(WR_LD_Mob_Number));Delay(2);
    }
    
    GPS_Config = GPS_OFF;
    Write_E2(EE_GPS_Config,(char*)&GPS_Config,sizeof(GPS_Config));
    Modem_setup.Retry_count = 3;
    Modem_setup.Limite_SMS = 100;
    Modem_setup.Limite_Server_data_usage = 75;
    Write_E2(EE_Retry_Count,(char*)&Modem_setup.Retry_count,sizeof(Modem_setup.Retry_count));
    Write_E2(EE_Limite_SMS,(char*)&Modem_setup.Limite_SMS,sizeof(Modem_setup.Limite_SMS));
    Write_E2(EE_Limite_Server_data_usage,(char*)&Modem_setup.Limite_Server_data_usage,sizeof(Modem_setup.Limite_Server_data_usage));
    
    
    Modem_setup.Per_Day_Server_data_usage = 0;
    Modem_setup.Per_Day_SMS_Count = 0;
    Modem_setup.SMS_Missing_count = 0;
    Modem_setup.Missing_Server_data = 0;
    Write_E2(EE_SMS_Missing_count,(char*)&Modem_setup.SMS_Missing_count,sizeof(Modem_setup.SMS_Missing_count));
    Write_E2(EE_Per_Day_Server_data_usage,(char*)&Modem_setup.Per_Day_Server_data_usage,sizeof(Modem_setup.Per_Day_Server_data_usage));
    Write_E2(EE_Per_Day_SMS_Count,(char*)&Modem_setup.Per_Day_SMS_Count,sizeof(Modem_setup.Per_Day_SMS_Count));
    Write_E2(EE_Missing_Server_data,(char*)&Modem_setup.Missing_Server_data,sizeof(Modem_setup.Missing_Server_data));
    
    ImmediateSentFlag = 0;
    SentDataCopied = 0;
    Com2StringSend_with_LRC("RSTAOK",6);
	LcdDisplay(TOP,   "                ");
	LcdDisplay(BOTTOM,"Defaults Loaded!");
	Delay1_1s(1);
	memset(LcdDispBuff,' ',32);
}
void ResetValues()
{
	memcpy(LcdDispBuff,"  RESETTING...  ",16);
	LcdDisplay(1,LcdDispBuff);
	memcpy(LcdDispBuff+16,"SIM   ",6);
	LcdDisplay(BOTTOM,LcdDispBuff+16);

	Com1StringSend("AT+QPOWD=0",10);
	Com1Bytesend(0x0d);
	Com1Bytesend(0x0a);
    HW_reset_Counter++;
	Delay(5000);Delay(5000);Delay(5000);
	Delay1_1s(1);
	memcpy(LcdDispBuff,"    SIM Check   ",16);	//Testing	AT+QPOWD=0 Quick Powerdown
	LcdDisplay(1,LcdDispBuff);
    ImmediateSentFlag = 0;
    SentDataCopied = 0;
    memset(ReceiveBuff1,' ',350);
    GPRSTimer = 0;
    GPRS_Flags1.SignalOk = 0;
    SignalLevel = 0;
    SignalTimer = 0;

    GPRS_Flag = 0;
    SMS_Flag = 0;
	Modem_Cmd_Sent = 0;
	Modem_Init_State = 0;
	GPRS_ResetFlag = 0;
    Modem_Init();                                       //  Contains Echo command and network search command
	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
	Do_Sim_Config();
	Delay1_1s(1);
}
void BigDelay(unsigned char DelayValue)
{
	unsigned int Int_Value;
	const unsigned int FIXEDDELAYVALUE = 50000;	
	while(DelayValue--)
	{
		for(Int_Value=0;Int_Value<FIXEDDELAYVALUE;Int_Value++);		
	}
	return;
}

void SIM_Balance_Check(void)							//Balance check
{
	unsigned char i = 0;
	LcdDisplay(TOP,"Balance Check   ");

	Com1StringSend("ATD",10);
	Com1Bytesend(0x0d);
	Com1Bytesend(0x0a);
	Delay1_1s(1);
	i = GPRS_Setup.MobileOptr;
	while(1)
	{
		if(GPRS_Setup.MobileOptr == 0)
			LcdDisplay(BOTTOM,"     AIRTEL     ");
		else if(GPRS_Setup.MobileOptr == 1)
			LcdDisplay(BOTTOM,"   TATA DOCOMO  ");
		else if(GPRS_Setup.MobileOptr == 2)
			LcdDisplay(BOTTOM,"      IDEA      ");
		else if(GPRS_Setup.MobileOptr == 3)
			LcdDisplay(BOTTOM,"     AIRCEL     ");
		else if(GPRS_Setup.MobileOptr == 4)
			LcdDisplay(BOTTOM,"      BSNL      ");

		KeySense();
		if(EnterKey)
		{
			 GPRS_Setup.MobileOptr = i;
				LcdDisplay(BOTTOM,"     STORED     ");
				Write_E2(EE_MobileOptr,(char*)&GPRS_Setup.MobileOptr,1);
				break;
		}		
	}
	Delay1_1s(1);
}

int main_display_loop =0;
void MainScreenDisplay()
{
    main_display_loop++;
    
    if((main_display_loop >1000)&&(main_display_loop <1200))
    {
        if((Modem_setup.Per_Day_Server_data_usage >= ((Modem_setup.Limite_Server_data_usage)*1000))&&(Modem_setup.Per_Day_SMS_Count>= Modem_setup.Limite_SMS))
        {
           sprintf(LcdDispBuff," SMS&GPRS OVER "); 
        }
        else if(Modem_setup.Per_Day_Server_data_usage>= ((Modem_setup.Limite_Server_data_usage)*1000))
        {
            sprintf(LcdDispBuff," GPRS DATA OVER"); 
        }
        else if(Modem_setup.Per_Day_SMS_Count>= Modem_setup.Limite_SMS)
        {
                sprintf(LcdDispBuff,"SMS LIMIT OVER ");      
        }
    }
    else if((main_display_loop >1200))
    {
        sprintf(LcdDispBuff,"               "); 
        main_display_loop =0;
    }
   
    else
    sprintf(LcdDispBuff,"R%d:%4u S%d:%4u",DisplayLocation+1,Rec[DisplayLocation].DisplayNo,DisplayLocation+1,Sent[DisplayLocation].DisplayNo);
    //sprintf(LcdDispBuff,"R%d:%4u S%d:%4u",DisplayLocation+1,GPS_Getting_Sec,DisplayLocation+1,Get_GPS_Timer);
    
/*   if((display_counter <= 100) && (Modem_Init_State == MODEM_OK))
    {
        if(!strncmp(Buffer_Value,"GSM",3))
            memcpy(LcdDispBuff+16,"  G   ",6);
        else if(!strncmp(Buffer_Value,"GPRS",4))
            memcpy(LcdDispBuff+16," 2G   ",6);
        else if(!strncmp(Buffer_Value,"EDGE",3))
            memcpy(LcdDispBuff+16,"  E   ",6);
        else if((!strncmp(Buffer_Value,"WCDMA",5))||(!strncmp(Buffer_Value,"HSDPA",5))||(!strncmp(Buffer_Value,"HSUPA",5))||(!strncmp(Buffer_Value,"HSPA+",5)))
            memcpy(LcdDispBuff+16," 3G   ",6);
        else if((!strncmp(Buffer_Value,"TDD LTE",7))||(!strncmp(Buffer_Value,"FDD LTE",7)))
            memcpy(LcdDispBuff+16," 4G   ",6);
        else 
            memcpy(LcdDispBuff+16,Buffer_Value,6);
    }
    else*/
        memcpy(LcdDispBuff+16,Modem_State_Buff,6);
    
    if(ImmediateSentFlag == 1){
        sprintf(LcdDispBuff+22,"L:%2u F9:%u ",SignalLevelCheck,GPRS_Flag);
    }
    else{
        if(Cur_Loc_Update == 0)
            sprintf(LcdDispBuff+22,"L:%2u F%d:%u ",SignalLevelCheck,Cur_Loc_Update+1,SMS_Flag);
        else if((Cur_Loc_Update == 1)||(Cur_Loc_Update == 2))
            sprintf(LcdDispBuff+22,"L:%2u F%d:%u ",SignalLevelCheck,Cur_Loc_Update+1,GPRS_Flag);
    }
    if(SimErrorFlag == 1)
    {
        if(One_Second_Counter > 500)
            LcdDisplay(BOTTOM,"   INSERT SIM   "); 
        else if(One_Second_Counter < 500)
            LcdDisplay(BOTTOM,"                "); 
    }
    else
    {
        LcdDisplay(BOTTOM,LcdDispBuff+16);
    }
    LcdDisplay(TOP,LcdDispBuff);
//    LcdDisplay(TOP,ReceiveBuff1);
}

unsigned int CalcLRC(char* ChecksumBuff,unsigned int ByteCount)                //	Calculating LRC for Received protocol	//	Here Received bytes and no.of received bytes are passed
{
	unsigned long LRC_Value = 0;
	unsigned int TempByteCount = 0;

	for(TempByteCount = 0;TempByteCount < ByteCount;TempByteCount++){
		LRC_Value = LRC_Value + ChecksumBuff[TempByteCount];
	}
	LRC_Value = LRC_Value % 100;												//	Extracting Last two digits for LRC value
	return(LRC_Value);															//	Returning LRC value
}

void CalcCRC(char *SPtr,unsigned int BytesCount)
{
	unsigned char CharAvail,a;
	unsigned char Counter,Counter2;
	unsigned int Check2Sum;
	Check2Sum=0xffff;
	for (Counter=0;Counter < BytesCount; Counter++) {
		CharAvail = *(SPtr);
		SPtr++;
		Check2Sum ^= CharAvail;
		for (Counter2=0;Counter2<8;Counter2++) {
			a = (Check2Sum & 1) ;
			Check2Sum  = Check2Sum / 2;
			if (a) 
			{
				Check2Sum ^= 0xa001;
			}
		}
	}
	MSBCSum = (Check2Sum >> 8 ) & 0xff;
	LSBCSum = (Check2Sum & 0xff);
}
void Data_Store(unsigned int Location,char *RecData)   //  Passing storage location No and data to be stored
{
    char AckBuff[63] = {0};
    if((Location >= 0)&&(Location <= 2))
    {
        if(Rec[Location].DisplayNo >= Rec[Location].MaxRecLimit){   //  To set record Overflowing condition when it crossed maximum record size
          Rec[Location].OverFlowFlag = 1;
          Rec[Location].Avail = Rec[Location].Avail % Rec[Location].MaxRecLimit;
        }
        Rec[Location].Pos = Rec[Location].Avail;                    //  Getting memory position for data storing
        Rec[Location].Avail ++;                                     //  Incrementing Available data No.
        Rec[Location].DisplayNo++;                                  //  Incrementing Record displaying number (2*Available data = Record displaying number)

        if(Rec[Location].DisplayNo >= Rec[Location].MaxRecLimit * 2){   //  To set record initialization purpose
            Rec[Location].Avail = 0; 
            Rec[Location].DisplayNo = 0; 
            Rec[Location].OverFlowFlag = 0;
            Rec[Location].InitFlag = 1;
            Write_E2(EE_Buff1_InitFlag+(1*Location),(char*)&Rec[Location].InitFlag,1);
            
        }
        
        Write_E2(EE_Buff1_RecAvail+(2*(Location)),(char*)& Rec[Location].Avail,2);            //  Storing Available Record Number
        Write_E2(EE_Buff1_RecDisplayNo+(2*(Location)),(char*)&Rec[Location].DisplayNo,sizeof(Rec[Location].DisplayNo));   //  Storing Record displaying number
        Write_E2((unsigned long)(Buff[Location].StartAddress+((unsigned long)(Rec[Location].Pos*Buff[Location].Length))),(char*)RecData,Buff[Location].Length);    // Actual Data Storing
        
        sprintf(AckBuff,"BUF%dOK*L%03d*S%02d*F9%02d*R1%04d*S1%04d*R2%04d*S2%04d*R3%04d*S3%04d*",Location+1,SignalLevelCheck,Modem_Init_State,GPRS_Flag,Rec[0].DisplayNo,Sent[0].DisplayNo,Rec[1].DisplayNo,Sent[1].DisplayNo,Rec[2].DisplayNo,Sent[2].DisplayNo);
        Com2StringSend_with_LRC(AckBuff,63);
    }
}

void Testing(void)
{
char TestBuff[30] = {0};
char TestBuff1[50] = {0};

    Com2Bytesend(0x0D);Com2Bytesend(0x0A);
    sprintf(TestBuff,"Length/%06d/%06d/%06d/",Buff[0].Length+2,Buff[1].Length+2,Buff[2].Length+2);
    Com2StringSend(TestBuff,28);  
    Com2Bytesend(0x0D);Com2Bytesend(0x0A);
    sprintf(TestBuff,"Address/%06lu/%06lu/%06lu/",Buff[0].StartAddress,Buff[1].StartAddress,Buff[2].StartAddress);
    Com2StringSend(TestBuff,28); 
    Com2Bytesend(0x0D);Com2Bytesend(0x0A);
    sprintf(TestBuff,"RecLim/%06lu/%06lu/%06lu/",Rec[0].MaxRecLimit,Rec[1].MaxRecLimit,Rec[2].MaxRecLimit);
    Com2StringSend(TestBuff,28); 
    Com2Bytesend(0x0D);Com2Bytesend(0x0A);
    Com2Bytesend(0x0D);Com2Bytesend(0x0A);
    sprintf(TestBuff1,"R1/%04d/R2/%04d/R3/%04d//S1/%04d/S2/%04d/S3/%04d/",Rec[0].DisplayNo,Rec[1].DisplayNo,Rec[2].DisplayNo,Sent[0].DisplayNo,Sent[1].DisplayNo,Sent[2].DisplayNo);
    Com2StringSend(TestBuff1,49); 
    Com2Bytesend(0x0D);Com2Bytesend(0x0A);
}
void Bank_1_Data_Reset()
{
    ImmediateSentFlag = 0;
    SentDataCopied = 0;
    Rec[0].Avail = 0;
    Write_E2(EE_Buff1_RecAvail,(char*)&Rec[0].Avail,sizeof(Rec[0].Avail));Delay(2);
    Sent[0].Avail = 0;
    Write_E2(EE_Buff1_SentAvail,(char*)&Sent[0].Avail,sizeof(Sent[0].Avail));Delay(2);
    Rec[0].DisplayNo = 0;
    Write_E2(EE_Buff1_RecDisplayNo,(char*)&Rec[0].DisplayNo,sizeof(Rec[0].DisplayNo));Delay(2);
    Sent[0].DisplayNo = 0;
    Write_E2(EE_Buff1_SentDisplayNo,(char*)&Sent[0].DisplayNo,sizeof(Sent[0].DisplayNo));Delay(2);
    Rec[0].InitFlag = 0;
    Write_E2(EE_Buff1_InitFlag,(char*)&Rec[0].InitFlag,sizeof(Rec[0].InitFlag));Delay(2);
    Rec[0].OverFlowFlag = 0;
}
void Bank_2_Data_Reset()
{
    ImmediateSentFlag = 0;
    SentDataCopied = 0;
    Rec[1].Avail = 0;
    Write_E2(EE_Buff2_RecAvail,(char*)&Rec[1].Avail,sizeof(Rec[1].Avail));Delay(2);
    Sent[1].Avail = 0;
    Write_E2(EE_Buff2_SentAvail,(char*)&Sent[1].Avail,sizeof(Sent[1].Avail));Delay(2);
    Rec[1].DisplayNo = 0;
    Write_E2(EE_Buff2_RecDisplayNo,(char*)&Rec[1].DisplayNo,sizeof(Rec[1].DisplayNo));Delay(2);
    Sent[1].DisplayNo = 0;
    Write_E2(EE_Buff2_SentDisplayNo,(char*)&Sent[1].DisplayNo,sizeof(Sent[1].DisplayNo));Delay(2);
    Rec[1].InitFlag = 0;
    Write_E2(EE_Buff2_InitFlag,(char*)&Rec[1].InitFlag,sizeof(Rec[1].InitFlag));Delay(2);
    Rec[1].OverFlowFlag = 0;
}
void Bank_3_Data_Reset()
{
    ImmediateSentFlag = 0;
    SentDataCopied = 0;
    Rec[2].Avail = 0;
    Write_E2(EE_Buff3_RecAvail,(char*)&Rec[2].Avail,sizeof(Rec[2].Avail));Delay(2);
    Sent[2].Avail = 0;
    Write_E2(EE_Buff3_SentAvail,(char*)&Sent[2].Avail,sizeof(Sent[2].Avail));Delay(2);
    Rec[2].DisplayNo = 0;
    Write_E2(EE_Buff3_RecDisplayNo,(char*)&Rec[2].DisplayNo,sizeof(Rec[2].DisplayNo));Delay(2);
    Sent[2].DisplayNo = 0;
    Write_E2(EE_Buff3_SentDisplayNo,(char*)&Sent[2].DisplayNo,sizeof(Sent[2].DisplayNo));Delay(2);
    Rec[2].InitFlag = 0;
    Write_E2(EE_Buff3_InitFlag,(char*)&Rec[2].InitFlag,sizeof(Rec[2].InitFlag));Delay(2);
    Rec[2].OverFlowFlag = 0;
}
void SMS_GPRS_Data_Reset()
{
    Modem_setup.Per_Day_Server_data_usage = 0;
    Modem_setup.Per_Day_SMS_Count = 0;
    Modem_setup.SMS_Missing_count = 0;
    Modem_setup.Missing_Server_data = 0;
    Write_E2(EE_Missing_Server_data,(char*)&Modem_setup.Missing_Server_data,sizeof(Modem_setup.Missing_Server_data));
    Write_E2(EE_SMS_Missing_count,(char*)&Modem_setup.SMS_Missing_count,sizeof(Modem_setup.SMS_Missing_count));
    Write_E2(EE_Per_Day_SMS_Count,(char*)&Modem_setup.Per_Day_SMS_Count,sizeof(Modem_setup.Per_Day_SMS_Count));
    Write_E2(EE_Per_Day_Server_data_usage,(char*)&Modem_setup.Per_Day_Server_data_usage,sizeof(Modem_setup.Per_Day_Server_data_usage));
}  
void Com2StringSend_with_LRC(char *LRCBuff,int Bytes)	/*  COM 2 string send with LRC */
{
char TempLRCBuff[Bytes+2];

    LRC = CalcLRC(LRCBuff,Bytes);                       //	LRC Calculation
    memcpy(TempLRCBuff,LRCBuff,Bytes);
    sprintf(TempLRCBuff+Bytes,"%02d",LRC);
    Com2StringSend("[",1);
    Com2StringSend(TempLRCBuff,Bytes+2);
    Com2StringSend("]",1);
}
void CurrentStatus(void)
{
    char CurrStatBuff[63] = {0};
    if(ImmediateSentFlag == 0)
        sprintf(CurrStatBuff,"STATUS*L%03d*S%02d*F%d%02d*R1%04d*S1%04d*R2%04d*S2%04d*R3%04d*S3%04d*",SignalLevelCheck,Modem_Init_State,Cur_Loc_Update+1,GPRS_Flag,Rec[0].DisplayNo,Sent[0].DisplayNo,Rec[1].DisplayNo,Sent[1].DisplayNo,Rec[2].DisplayNo,Sent[2].DisplayNo);
    else if(ImmediateSentFlag == 1)
        sprintf(CurrStatBuff,"STATUS*L%03d*S%02d*F9%02d*R1%04d*S1%04d*R2%04d*S2%04d*R3%04d*S3%04d*",SignalLevelCheck,Modem_Init_State,GPRS_Flag,Rec[0].DisplayNo,Sent[0].DisplayNo,Rec[1].DisplayNo,Sent[1].DisplayNo,Rec[2].DisplayNo,Sent[2].DisplayNo);
    Com2StringSend_with_LRC(CurrStatBuff,63);
}

void Store_Mob_Number(char *WR_Mob_Num_Buff,unsigned int BuffCnt)
{
    char Mob_No_order = 0;
    char WR_Mob_DataBuff[10] = {0};
    char WR_Mob_Number[15] = {"               "};
    int NumberCheck,Mob_Digit = 0;

    memcpy(WR_Mob_DataBuff,WR_Mob_Num_Buff+4,2);
    Mob_No_order = (atoi(WR_Mob_DataBuff) - 1);

    for(NumberCheck=6;NumberCheck<=22;NumberCheck++)
    {
        if(WR_Mob_Num_Buff[NumberCheck] != '*')
        {
            WR_Mob_Number[Mob_Digit] = WR_Mob_Num_Buff[NumberCheck];
            Mob_Digit++;
        }
    }
    Write_E2(EE_SMS_Number_1+(Mob_No_order*15),(char*)&WR_Mob_Number,sizeof(WR_Mob_Number));Delay(2);
    Com2StringSend_with_LRC("MWR OK",6);
//    Com2StringSend(WR_Mob_Number,15);
//    Com2Bytesend(0x0d);
//    Com2Bytesend(0x0a);
}
void Store_Set_SMS_GPRS_Retry(char *WR_Mob_Num_Buff,unsigned int BuffCnt)
{
   Modem_setup.Retry_count =  WR_Mob_Num_Buff[7] -48;
    
    Write_E2(EE_Retry_Count,(char*)&Modem_setup.Retry_count,sizeof(Modem_setup.Retry_count));Delay(2);
    Com2StringSend_with_LRC("SSGRWR OK",9);
 
}
void Store_Set_SMS_Count(char *WR_Mob_Num_Buff,unsigned int BuffCnt)
{
    
    char WR_Mob_DataBuff[3] = {0};
    WR_Mob_DataBuff[0] = WR_Mob_Num_Buff[6] ;
    WR_Mob_DataBuff[1] = WR_Mob_Num_Buff[7] ;
    WR_Mob_DataBuff[2] = WR_Mob_Num_Buff[8] ;
    Modem_setup.Limite_SMS =(int) atoi(WR_Mob_DataBuff);
    
    Write_E2(EE_Limite_SMS,(char*)&Modem_setup.Limite_SMS,sizeof(Modem_setup.Limite_SMS));Delay(2);
    Com2StringSend_with_LRC("SSCWR OK",8);
 
}

void Store_Set_GPRS_LIMIT(char *WR_Mob_Num_Buff,unsigned int BuffCnt)
{
    
    char WR_Mob_DataBuff[6] = {0};
    WR_Mob_DataBuff[0] = WR_Mob_Num_Buff[6] ;
    WR_Mob_DataBuff[1] = WR_Mob_Num_Buff[7] ;
    WR_Mob_DataBuff[2] = WR_Mob_Num_Buff[8] ;
    WR_Mob_DataBuff[3] = WR_Mob_Num_Buff[9] ;
    Modem_setup.Limite_Server_data_usage = (long)atol(WR_Mob_DataBuff);
    Write_E2(EE_Limite_Server_data_usage,(char*)&Modem_setup.Limite_Server_data_usage,sizeof(Modem_setup.Limite_Server_data_usage));Delay(2);
    Com2StringSend_with_LRC("SGLWR OK",8);
    
}
void Read_Mob_Number(int RD_Mob_Order)
{
    char RD_Mob_Number[15] = {"               "};
    char Send_RD_Num_Buff[23] = {0};

    Read_E2((char*)&RD_Mob_Number,EE_SMS_Number_1+(RD_Mob_Order*15),sizeof(RD_Mob_Number));

    memcpy(Send_RD_Num_Buff,"MRD*",4);
    sprintf(Send_RD_Num_Buff+4,"%02d",(RD_Mob_Order+1));
    Send_RD_Num_Buff[6] = '*';
    memcpy(Send_RD_Num_Buff+7,RD_Mob_Number,3);
    Send_RD_Num_Buff[10] = '*';
    memcpy(Send_RD_Num_Buff+11,RD_Mob_Number+3,12);

    Com2StringSend_with_LRC(Send_RD_Num_Buff,23);
    Com2Bytesend(0x0d);
    Com2Bytesend(0x0a);
}
void Read_Set_SMS_GPRS_Retry()
{
   
    char Send_Buff1[23] = {0};

    
    Read_E2((char*)&Modem_setup.Retry_count,EE_Retry_Count,sizeof(Modem_setup.Retry_count));
    
    memcpy(Send_Buff1,"SSGRRD*",7);
    sprintf(Send_Buff1+7,"%1d",Modem_setup.Retry_count);
    Send_Buff1[8] = '*';
   

    Com2StringSend_with_LRC(Send_Buff1,9);
//    Com2Bytesend(0x0d);
//    Com2Bytesend(0x0a);
}
void Read_Set_SMS_Count()
{
    
    char Send_Buff1[23] = {0};

    
    Read_E2((char*)&Modem_setup.Limite_SMS,EE_Limite_SMS,sizeof(Modem_setup.Limite_SMS));
    
    memcpy(Send_Buff1,"SSCRD*",6);
    sprintf(Send_Buff1+6,"%3u",Modem_setup.Limite_SMS);
    Send_Buff1[9] = '*';
   

    Com2StringSend_with_LRC(Send_Buff1,10);
//    Com2Bytesend(0x0d);
//    Com2Bytesend(0x0a);
}
void Read_Set_GPRS_LIMIT()
{
    
    char Send_Buff1[23] = {0};

    
    Read_E2((char*)&Modem_setup.Limite_Server_data_usage,EE_Limite_Server_data_usage,sizeof(Modem_setup.Limite_Server_data_usage));
    
    memcpy(Send_Buff1,"SGLRD*",6);
    sprintf(Send_Buff1+6,"%4lu",Modem_setup.Limite_Server_data_usage);
    Send_Buff1[10] = '*';
   

    Com2StringSend_with_LRC(Send_Buff1,11);
//    Com2Bytesend(0x0d);
//    Com2Bytesend(0x0a);
}
void Read_View_SMS_Count()
{
    
    char Send_Buff1[23] = {0};

    
    Read_E2((char*)&Modem_setup.Per_Day_SMS_Count,EE_Per_Day_SMS_Count,sizeof(Modem_setup.Per_Day_SMS_Count));
    Read_E2((char*)&Modem_setup.Limite_SMS,EE_Limite_SMS,sizeof(Modem_setup.Limite_SMS));
    Read_E2((char*)&Modem_setup.SMS_Missing_count,EE_SMS_Missing_count,sizeof(Modem_setup.SMS_Missing_count));
    
    
    memcpy(Send_Buff1,"VSCRD*",6);
    sprintf(Send_Buff1+6,"%03u/%03u%03u",Modem_setup.Per_Day_SMS_Count,Modem_setup.Limite_SMS,Modem_setup.SMS_Missing_count);
    Send_Buff1[16] = '*';
   

    Com2StringSend_with_LRC(Send_Buff1,17);
//    Com2Bytesend(0x0d);
//    Com2Bytesend(0x0a);
}
void Read_View_GPRS_LIMIT()
{
    
    char Send_Buff1[23] = {0};

    
    Read_E2((char*)&Modem_setup.Per_Day_Server_data_usage,EE_Per_Day_Server_data_usage,sizeof(Modem_setup.Per_Day_Server_data_usage));
    Read_E2((char*)&Modem_setup.Limite_Server_data_usage,EE_Limite_Server_data_usage,sizeof(Modem_setup.Limite_Server_data_usage));
    Read_E2((char*)&Modem_setup.Missing_Server_data,EE_Missing_Server_data,sizeof(Modem_setup.Missing_Server_data));
    memcpy(Send_Buff1,"VGLRD*",6);
    sprintf(Send_Buff1+6,"%07lu/%04lu%03u",(long)((Modem_setup.Per_Day_Server_data_usage)),(long)(Modem_setup.Limite_Server_data_usage),(int)(Modem_setup.Missing_Server_data));
    Send_Buff1[21] = '*';
   

    Com2StringSend_with_LRC(Send_Buff1,22);
//    Com2Bytesend(0x0d);
//    Com2Bytesend(0x0a);
}
void Record_Update_Succeeded(unsigned int Loc)   //  Once record update success, clear all variables and equalize its corresponding parameters
{
    GPRS_Flag = 0;
    GPRS_Flags1.SignalOk = 0;
    SignalTimer = 0;
    SentDataCopied = 0;
    SMS_Retry_count =0;
    
    
    if(ImmediateSentFlag == 1){
        ImmediateSentFlag = 0;
    }
    else
    {
        Cur_Loc_Update++;
        if(Cur_Loc_Update > 2)
            Cur_Loc_Update = 0;
        Sent[Loc].Avail += 1;
        Write_E2(EE_Buff1_SentAvail+(2*Loc),(char*)&Sent[Loc].Avail,sizeof(Sent[Loc].Avail));Delay(10);
        Sent[Loc].DisplayNo++;
        Write_E2(EE_Buff1_SentDisplayNo+(2*Loc),(char*)&Sent[Loc].DisplayNo,sizeof(Sent[Loc].DisplayNo));Delay(20);
        if(Sent[Loc].DisplayNo >= (Rec[Loc].MaxRecLimit*2))
        {
            Sent[Loc].Avail = 0;
            Sent[Loc].DisplayNo = 0;
            Rec[Loc].InitFlag = 0;
            Rec[Loc].OverFlowFlag = 0;
            Write_E2(EE_Buff1_InitFlag+(1*Loc),(char*)&Rec[Loc].InitFlag,sizeof(Rec[Loc].InitFlag));
            Write_E2(EE_Buff1_SentAvail+(2*Loc),(char*)&Sent[Loc].Avail,sizeof(Sent[Loc].Avail));Delay(10);                    
            Write_E2(EE_Buff1_SentDisplayNo+(2*Loc),(char*)&Sent[Loc].DisplayNo,sizeof(Sent[Loc].DisplayNo));Delay(20);                    
        }
    }
    SMS_No_Position = SMS_Flag = SMS_Response_Flag = 0;
    memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
}

void Retry_Record_Update_Succeeded(unsigned int Loc)   //  Once record update success, clear all variables and equalize its corresponding parameters
{
    GPRS_Flag = 0;
    GPRS_Flags1.SignalOk = 0;
    SignalTimer = 0;
    SentDataCopied = 0;
    SMS_Retry_count =0;
    SMS_No_Position = SMS_Flag = SMS_Response_Flag = 0;
    
//    if(ImmediateSentFlag == 1){
//        ImmediateSentFlag = 0;
//    }
//    else
    {
        Cur_Loc_Update++;
        if(Cur_Loc_Update > 2)
            Cur_Loc_Update = 0;
        Sent[Loc].Avail += 1;
        Write_E2(EE_Buff1_SentAvail+(2*Loc),(char*)&Sent[Loc].Avail,sizeof(Sent[Loc].Avail));Delay(10);
        Sent[Loc].DisplayNo++;
        Write_E2(EE_Buff1_SentDisplayNo+(2*Loc),(char*)&Sent[Loc].DisplayNo,sizeof(Sent[Loc].DisplayNo));Delay(20);
        if(Sent[Loc].DisplayNo >= (Rec[Loc].MaxRecLimit*2))
        {
            Sent[Loc].Avail = 0;
            Sent[Loc].DisplayNo = 0;
            Rec[Loc].InitFlag = 0;
            Rec[Loc].OverFlowFlag = 0;
            Write_E2(EE_Buff1_InitFlag+(1*Loc),(char*)&Rec[Loc].InitFlag,sizeof(Rec[Loc].InitFlag));
            Write_E2(EE_Buff1_SentAvail+(2*Loc),(char*)&Sent[Loc].Avail,sizeof(Sent[Loc].Avail));Delay(10);                    
            Write_E2(EE_Buff1_SentDisplayNo+(2*Loc),(char*)&Sent[Loc].DisplayNo,sizeof(Sent[Loc].DisplayNo));Delay(20);                    
        }
    }
    memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
}
unsigned long typecase_String_to_long(char *Buff,unsigned int BuffCnt )
{
    char Local_string [10] = {0};
    long Local_value = 0;
    char Local_loop;
   
    for(Local_loop = 0; Local_loop < BuffCnt;Local_loop++)
    {
        Local_string[Local_loop] = Buff[Local_loop];
       
    }
    Local_value = atol(Local_string);
    
    return Local_value;

}