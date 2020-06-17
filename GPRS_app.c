#include "GPRS_app.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

//#include "Includes.h" //  3rd time comment
//#include "Includes.h"
//#include "Includes.h"

extern char HW_reset_Counter;
unsigned char CurrentWeightFlag = 0;

/*************************************************************************
Function Name: GPRS_Update()
Parameters	 : None 
Return		 : None
Description  : Entire process of updating data to server is handled.
*************************************************************************/
void GPRS_Update()
{
	GPRS_Flags1.ModemOK = 1;

    if(!GPRS_Flags1.ModemOK)
		GPRSInitialCheck();
	else
	{
		if(!GPRS_Flags1.SignalOk)
		{
            CheckSignalStrength();
		}
        else
		{
           
            if(SentDataCopied == 1)
            {
                if(Cur_Loc_Update == 0)
                {
                    Send_SMS(Cur_Loc_Update,ImmediateSentBuff,Buff[Cur_Loc_Update].Length);
                    
                }
                else if((Cur_Loc_Update == 1) || (Cur_Loc_Update == 2))
                {
                    Data_Update(Cur_Loc_Update,ImmediateSentBuff,Buff[Cur_Loc_Update].Length);
                    
                }
               
            }
            else if(ImmediateSentFlag == 1)
            {
                Data_Update(Cur_Loc_Update,ImmediateSentBuff1,Buff[2].Length);
            }
            else
            {
                if((Cur_Loc_Update == 0)||(Cur_Loc_Update == 1)||(Cur_Loc_Update == 2))
                {
                    if(Rec[Cur_Loc_Update].DisplayNo != Sent[Cur_Loc_Update].DisplayNo)  
                        // Here != mean "Rec[Cur_Loc_Update].DisplayNo = Maximum and then 0 state" it will work properly
                        // IN this case "10 != 889"
                    {
                        if(Rec[Cur_Loc_Update].InitFlag == 1)
                        {
                            if(Rec[Cur_Loc_Update].DisplayNo < Rec[Cur_Loc_Update].MaxRecLimit)
                            {
                                if((((Rec[Cur_Loc_Update].MaxRecLimit * 2)- Sent[Cur_Loc_Update].DisplayNo)+Rec[Cur_Loc_Update].DisplayNo) < Rec[Cur_Loc_Update].MaxRecLimit)
                                {
                                    Sent[Cur_Loc_Update].Avail = (Sent[Cur_Loc_Update].Avail % Rec[Cur_Loc_Update].MaxRecLimit);
                                    Read_E2((char*)&ImmediateSentBuff,(unsigned long)(Buff[Cur_Loc_Update].StartAddress+((unsigned long)(Sent[Cur_Loc_Update].Avail * Buff[Cur_Loc_Update].Length))),Buff[Cur_Loc_Update].Length);
                                    SentDataCopied = 1;
                                    GPRS_ResetFlag = 0;
                                    GPRS_Flag = 0;
                                    SMS_Flag = 0;
                                }
                                else
                                {
                                    Sent[Cur_Loc_Update].Avail = Sent[Cur_Loc_Update].DisplayNo = (Rec[Cur_Loc_Update].MaxRecLimit * 2) - (Rec[Cur_Loc_Update].MaxRecLimit - Rec[Cur_Loc_Update].DisplayNo);
                                    Sent[Cur_Loc_Update].Avail = (Sent[Cur_Loc_Update].Avail % Rec[Cur_Loc_Update].MaxRecLimit);
                                    Read_E2((char*)&ImmediateSentBuff,(unsigned long)(Buff[Cur_Loc_Update].StartAddress+((unsigned long)(Sent[Cur_Loc_Update].Avail * Buff[Cur_Loc_Update].Length))),Buff[Cur_Loc_Update].Length);
                                    SentDataCopied = 1;
                                    GPRS_ResetFlag = 0;
                                    GPRS_Flag = 0;
                                    SMS_Flag = 0;
                                }
                            }
                            else if(Rec[Cur_Loc_Update].DisplayNo >= Rec[Cur_Loc_Update].MaxRecLimit)
                            {
                                Sent[Cur_Loc_Update].Avail = Sent[Cur_Loc_Update].DisplayNo = (Rec[Cur_Loc_Update].DisplayNo - Rec[Cur_Loc_Update].MaxRecLimit);
                                Rec[Cur_Loc_Update].InitFlag = 0;
                                Write_E2(EE_Buff1_InitFlag+(1*(Cur_Loc_Update-1)),(char*)&Rec[Cur_Loc_Update].InitFlag,1);
                                Sent[Cur_Loc_Update].Avail = (Sent[Cur_Loc_Update].Avail % Rec[Cur_Loc_Update].MaxRecLimit);
                                Read_E2((char*)&ImmediateSentBuff,(unsigned long)(Buff[Cur_Loc_Update].StartAddress+((unsigned long)(Sent[Cur_Loc_Update].Avail * Buff[Cur_Loc_Update].Length))),Buff[Cur_Loc_Update].Length);
                                SentDataCopied = 1;
                                GPRS_ResetFlag = 0;
                                GPRS_Flag = 0;
                                SMS_Flag = 0;
                            }
                        }
                        else
                        {
                            if(Rec[Cur_Loc_Update].DisplayNo > Sent[Cur_Loc_Update].DisplayNo)
                            {
                                if((Rec[Cur_Loc_Update].DisplayNo - Sent[Cur_Loc_Update].DisplayNo) > Rec[Cur_Loc_Update].MaxRecLimit)
                                        Sent[Cur_Loc_Update].Avail = Sent[Cur_Loc_Update].DisplayNo = (Rec[Cur_Loc_Update].DisplayNo - Rec[Cur_Loc_Update].MaxRecLimit);
                            }
                            if((Rec[Cur_Loc_Update].OverFlowFlag == 0) && (Rec[Cur_Loc_Update].DisplayNo <= Rec[Cur_Loc_Update].MaxRecLimit))
                            {
                                Sent[Cur_Loc_Update].Avail = (Sent[Cur_Loc_Update].Avail % Rec[Cur_Loc_Update].MaxRecLimit);
                                Read_E2((char*)&ImmediateSentBuff,(unsigned long)(Buff[Cur_Loc_Update].StartAddress+((unsigned long)(Sent[Cur_Loc_Update].Avail * Buff[Cur_Loc_Update].Length))),Buff[Cur_Loc_Update].Length);
                                SentDataCopied = 1;
                                GPRS_ResetFlag = 0;
                                GPRS_Flag = 0;
                                SMS_Flag = 0;
                            }
                            else if((Rec[Cur_Loc_Update].OverFlowFlag == 1) && (Rec[Cur_Loc_Update].DisplayNo > Rec[Cur_Loc_Update].MaxRecLimit))
                            {
                                Sent[Cur_Loc_Update].Avail = (Sent[Cur_Loc_Update].Avail % Rec[Cur_Loc_Update].MaxRecLimit);
                                Read_E2((char*)&ImmediateSentBuff,(unsigned long)(Buff[Cur_Loc_Update].StartAddress+((unsigned long)(Sent[Cur_Loc_Update].Avail * Buff[Cur_Loc_Update].Length))),Buff[Cur_Loc_Update].Length);
                                SentDataCopied = 1;
                                GPRS_ResetFlag = 0;
                                GPRS_Flag = 0;
                                SMS_Flag = 0;
                            }
                        }
                    }
                    else
                    {
                        Cur_Loc_Update++;
                        if(Cur_Loc_Update > 2)
                            Cur_Loc_Update = 0;
                    }
                }
                GPRSTimer = 0;
            }
		}
	}
}
/*************************************************************************
Function Name: CheckSignalStrength
Parameters	 : None
Return		 : None
Description  : Signal strength is read from the Modem
*************************************************************************/
void CheckSignalStrength()
{
	char SignalBuff[3] = {0};
   // GPRS_Flag = 0;			

	SignalTimer++;
	if((SignalTimer == 1) && (Modem_Request == 0)&&(GPRS_Flag == 0)&&(SMS_Flag == 0))
	{
		memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
		Com1StringSend("at+csq",6);
		Com1Bytesend(0x0d);
		Com1Bytesend(0x0a);
	}
	else if(SignalTimer >= 500)
	{
		if(ReceiveCounter1 > 10)
		{
			if(!strncmp(ReceiveBuff1+3,"CSQ",3))
			{
                memcpy(SignalBuff,ReceiveBuff1+8,2);
				//memcpy(LcdDispBuff,SignalBuff,3);
				SignalLevelCheck = atoi(SignalBuff);
                memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
			}
		}
        if((SignalLevelCheck > 0) && (SignalLevelCheck <= 31))
        {
            GPRS_Flags1.SignalOk = 1;
            Modem_Request = 0;
            SignalTimer = 0;
        }
        else
        {
            SignalTimer = 0;
            Modem_Request = 0;
            GPRS_Flags1.SignalOk = 0;	
            SignalLevelCheck = 0;
        }
	}
}

/*************************************************************************
Function Name: GPRSInitialCheck
Parameters	 : None
Return		 : None
Description  : 
*************************************************************************/
unsigned long loop;
unsigned int loop_cunt =0;
void GPRSInitialCheck()
{
	memset(ReceiveBuff1,' ',350);
	ReceiveCounter1 = 0;
	if(GPRS_Flags1.SendCheck)
	{
		GPRS_Flags1.SendCheck = 0;
		Com1StringSend("at",2);
		Com1Bytesend(0x0d);
		Com1Bytesend(0x0a);
	}
	if(ReceiveBuff1[2]=='O' && ReceiveBuff1[3]=='K')
		GPRS_Flags1.ModemOK = 1;
}

void Data_Update(unsigned int Loc,char *SendData,unsigned int SendData_Length)
{
    char SendData1[500] = {0};
    Read_E2((char*)&Modem_setup.Limite_Server_data_usage,EE_Limite_Server_data_usage,sizeof(Modem_setup.Limite_Server_data_usage));
    if(Modem_setup.Per_Day_Server_data_usage>= ((Modem_setup.Limite_Server_data_usage)*1000))
    {
        GPRS_Flag = 10;
        //Retry_Record_Update_Succeeded(Loc);
      //  return;
    }
	if(GPRS_Flag == 0)
	{

            if(QIACT_Flag == 1){
            GPRS_Flag = 6;
            }
            else{
                GPRS_ResetFlag = 0;
                GPRS_Flag = 1;        
            }     

	}
	else if(GPRS_Flag == 1)
	{
	    GPRS_Flag = 2;
		memset(Send_Buff1,' ',80);
		strcpy(Send_Buff1,"AT+QIACT=1");
		memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
		Com1StringSend(Send_Buff1,10);
		Com1Bytesend(0x0D);
		Com1Bytesend(0x0A);
        Reset_flag = 0;
	}
	else if(GPRS_Flag == 2)
	{
//        LcdDisplay(TOP,  ReceiveBuff1);loop = 999999;while(loop--);
          if(!strncmp(ReceiveBuff1+2,"OK",2))
            { 
                     GPRS_Flag = 4;
                     Reset_flag = 0;
            }
            else if(!strncmp(ReceiveBuff1+2,"ERROR",5))
            {
                GPRS_Flag = 3;
                memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
                memset(Send_Buff1,' ',80);
                strcpy(Send_Buff1,"AT+QIDEACT=1");
                Com1StringSend(Send_Buff1,12);
                Com1Bytesend(0x0D);
                Com1Bytesend(0x0A);
            }

       
	}
	else if(GPRS_Flag == 3)
	{

         if(!strncmp(ReceiveBuff1+2,"OK",2))
            {
                GPRS_Flag = 1;
               
            }
        
	}
	else if(GPRS_Flag == 4)
	{
	    GPRS_Flag = 5;
		memset(Send_Buff1,' ',80);
        memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
		strcpy(Send_Buff1,"AT+QIOPEN=1,0,\"TCP\",\"www.lcspayload.in\",80,0,1");
		
		Com1StringSend(Send_Buff1,46);
		Com1Bytesend(0x0D);
		Com1Bytesend(0x0A);	
         Delay1_1s(1);
        
	}
	else if(GPRS_Flag == 5)
	{
        if(!strncmp(ReceiveBuff1+2,"OK",2))//((!strncmp(ReceiveBuff1+2,"OK",2))&&(!strncmp(ReceiveBuff1+2,"+QIOPEN:",8)))
            {
            GPRS_Flag = 6;
            QIACT_Flag = 1;
            }

	}
	else if(GPRS_Flag == 6)
	{
		GPRS_Flag = 7;
		memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
		Com1StringSend("AT+QISEND=0",11);
		Com1Bytesend(0x0D);
		Com1Bytesend(0x0A);
	}
	else if(GPRS_Flag == 7)
	{
        
        int loop1 = 0;
        for(loop1 = 0;loop1<100;loop1++)
        {
            if(ReceiveBuff1[loop1] == '>')
                {
                    GPRS_Flag = 8;
                    break;
                }
            else if(!strncmp(ReceiveBuff1+2,"ERROR",5))
             {
                         QIACT_Flag = GPRS_Flag = 0;                 //  Working
                         memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
                         memset(Send_Buff1,' ',80);
                         strcpy(Send_Buff1,"AT+QICLOSE=0");		
                         Com1StringSend(Send_Buff1,12);
                         Com1Bytesend(0x0D);Com1Bytesend(0x0A);
                         loop = 199999;while(loop--);loop = 199999;while(loop--);
                         memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
                         memset(Send_Buff1,' ',80);
                         strcpy(Send_Buff1,"AT+QIDEACT=1");
                         Com1StringSend(Send_Buff1,12);
                         Com1Bytesend(0x0D);
                         Com1Bytesend(0x0A);
                         loop = 199999;while(loop--);loop = 199999;while(loop--);
                         break;
             }
        }
       

//             Reset_flag1 = 65353;
//             LcdDisplay(TOP, ReceiveBuff1);while(Reset_flag1--);
//             Reset_flag1 = 65353;while(Reset_flag1--); Reset_flag1 = 65353;while(Reset_flag1--);
//        if(Reset_flag1>10)
//        {
//            GPRS_ResetFlag = 1;
//            Reset_flag1 = 0;
//        }
	}
	else if(GPRS_Flag == 8)
	{
		 
		
        GPRS_Flag = 9;
		memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
		memset(Send_Buff1,' ',80);
//        memcpy(ReceiveBuff1+2,"ERROR",5);
//        sprintf(Send_Buff1,"RC=%u SRC=%u",GPRS_Retry_count,Modem_setup.Retry_count);
//        LcdDisplay(TOP, Send_Buff1);
//        Delay1_1s(1);
    	//strcpy(Send_Buff1,"POST /GPSDATAWebService/GPSDataService.asmx/BinData5100 HTTP/1.1");
        strcpy(Send_Buff1,"POST /GPSDATAWebService/GPSDataService.asmx/ReachStacker HTTP/1.1");
		Com1StringSend(Send_Buff1,65);
        
		Com1Bytesend(0x0D);
		Com1Bytesend(0x0A);
    	strcpy(Send_Buff1,"Host: www.lcspayload.in");
		Com1StringSend(Send_Buff1,23);
		Com1Bytesend(0x0D);
		Com1Bytesend(0x0A);
		strcpy(Send_Buff1,"Content-Type: application/x-www-form-urlencoded");
		Com1StringSend(Send_Buff1,47);
		Com1Bytesend(0x0D);
		Com1Bytesend(0x0A);

        strcpy(Send_Buff1,"Content-Length:");
		Com1StringSend(Send_Buff1,15);
		sprintf(Send_Buff1,"%03u",(SendData_Length+11));
		Com1StringSend(Send_Buff1,3);
		Com1Bytesend(0x0D);
		Com1Bytesend(0x0A);
		Com1Bytesend(0x0D);
		Com1Bytesend(0x0A);
    	memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
        
		strcpy(Send_Buff1,"protocol=");
		Com1StringSend(Send_Buff1,9);
        memcpy(SendData1,"[",1);
        memcpy(SendData1+1,SendData,SendData_Length);
        memcpy(SendData1+(SendData_Length+1),"]",1);
    	Com1StringSend(SendData1,SendData_Length+2);        //  Data sending here //
        Com1Bytesend(0x0D);
		Com1Bytesend(0x0A);
		Com1Bytesend(0x0D);
		Com1Bytesend(0x0A);
		Com1Bytesend(0x1A);
        
        GPRS_Retry_count++; 
            if( GPRS_Retry_count > Modem_setup.Retry_count)
            {
                GPRS_Retry_count = 0;
                GPRS_Flag = 10;
                GPRS_Uart1Flag = 0;
                memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
               // Retry_Record_Update_Succeeded(Loc);   //immediate data involve
                Modem_setup.Missing_Server_data++;
                Write_E2(EE_Missing_Server_data,(char*)&Modem_setup.Missing_Server_data,sizeof(Modem_setup.Missing_Server_data));
            }
          Modem_setup.Per_Day_Server_data_usage += 2;
          Write_E2(EE_Per_Day_Server_data_usage,(char*)&Modem_setup.Per_Day_Server_data_usage,sizeof(Modem_setup.Per_Day_Server_data_usage));
    
	}
	else if(GPRS_Flag == 9)         //  checking Acknowledgement from server
	{
        if((!strncmp(ReceiveBuff1,"LCS**OK",7))&&(GPRS_Uart1Flag == 1))
        {
            GPRS_Flag = 10;
			GPRS_Uart1Flag = 0;
			memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
        }
        else if((!strncmp(ReceiveBuff1,"LCS*NOK",7))&&(GPRS_Uart1Flag == 1))
        {
   			GPRS_Flag = 0;
			GPRS_Uart1Flag = 0;
			memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
        }
       
	}
	else if(GPRS_Flag == 10)
	{
        Record_Update_Succeeded(Loc);                   //  Clearing all used variables, incrementing serial number and so on //
        HW_reset_Counter = 0;

	}

    if(GPRSTimer > 41000) //41000
    {
        if(GPRS_Flag == 7)
            Com1Bytesend(0x1B);
        GPRSTimer = 0;
        GPRS_ResetFlag = 1;
        //Cur_Loc_Update = 0;
            return;
    }
}

void GPRS_Close_State(void)
{
    unsigned int close_loop_flag = 0;
    
    GPRS_Close_flag = 0;
   
    for(close_loop_flag = 0; close_loop_flag < ReceiveCounter2; close_loop_flag++)
    {
        if((ReceiveBuff2[close_loop_flag] == 'c')&&(ReceiveBuff2[close_loop_flag+1] == 'l')&&
                (ReceiveBuff2[close_loop_flag+2] == 'o')&&(ReceiveBuff2[close_loop_flag+3] == 's')&&
                (ReceiveBuff2[close_loop_flag+4] == 'e'))
        {
            GPRS_Close_flag = 1;
            break;
        }
        else if((ReceiveBuff2[close_loop_flag] == 'E')&&(ReceiveBuff2[close_loop_flag+1] == 'R')&&
                (ReceiveBuff2[close_loop_flag+2] == 'R')&&(ReceiveBuff2[close_loop_flag+3] == 'O')&&
                (ReceiveBuff2[close_loop_flag+4] == 'R'))
        {
            GPRS_Close_flag = 2;
            break;
        }
       
    }
        if((GPRS_Close_flag == 1)&&(SMS_Flag == 0))
        {
                GPRS_Close_flag = 0;
                ReceiveCounter2 = 0;
                memset(ReceiveBuff2,' ',1350);
                memset(ReceiveBuff1,' ',350);
                ReceiveCounter1 = 0;
                memset(Send_Buff1,' ',80);
                strcpy(Send_Buff1,"AT+QICLOSE=0");		
                Com1StringSend(Send_Buff1,12);
                Com1Bytesend(0x0D);
                Com1Bytesend(0x0A);
                Delay1_1s(5);

                    memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
                    memset(Send_Buff1,' ',80);
                    strcpy(Send_Buff1,"AT+QIDEACT=1");
                    Com1StringSend(Send_Buff1,12);
                    Com1Bytesend(0x0D);
                    Com1Bytesend(0x0A);
                    Delay1_1s(5);
        }
        else if(GPRS_Close_flag == 2)
        {
            GPRS_Close_flag = 0;
             QIACT_Flag = GPRS_Flag = 0;
              memset(ReceiveBuff2,' ',1350);
              ReceiveCounter2 = 0;
            
        }

}

