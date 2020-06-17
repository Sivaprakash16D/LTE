    #include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "SMS_Functions.h"
#include "MemoryDefines.h"

void Send_SMS(unsigned int SMS_Loc,char *SMSData,unsigned int SMS_Length)
{
    unsigned char Mobile_Number[15] = {0};
    unsigned int Mob_Digits = 0;
    Read_E2((char*)&Modem_setup.SMS_Missing_count,EE_SMS_Missing_count,sizeof(Modem_setup.SMS_Missing_count));
    if(Modem_setup.Per_Day_SMS_Count >= Modem_setup.Limite_SMS)
        {
//            SMS_Flag = 8;
//            SMS_No_Position = 4;
            Retry_Record_Update_Succeeded(SMS_Loc);
            Modem_setup.SMS_Missing_count++;
            Write_E2(EE_SMS_Missing_count,(char*)&Modem_setup.SMS_Missing_count,sizeof(Modem_setup.SMS_Missing_count));
            return;
        }
    if(SMS_Flag == 0)
    {
        memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
        Com1StringSend("AT+CMGF=1",9);                     //   This is used to set SMS in text mode,if = 0 it is in PDU mode
        Com1Bytesend(0x0D);
        Com1Bytesend(0x0A);
        SMS_Flag = 1;
    }
    else if(SMS_Flag == 1)
    {
        if(!strncmp(ReceiveBuff1+2,"OK",2))//if(ReceiveBuff1[2]=='O' && ReceiveBuff1[3]=='K')
        {
            SMS_Flag = 2;
        }
    }
    else if(SMS_Flag == 2)
    {
        memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
        Com1StringSend("AT+CSCS=\"GSM\"",13);              //    This is used to set character set in English
        Com1Bytesend(0x0d);
        Com1Bytesend(0x0a);
        SMS_Flag = 3;
    }
    else if(SMS_Flag == 3)
    {
        if(!strncmp(ReceiveBuff1+2,"OK",2))//if(ReceiveBuff1[2]=='O' && ReceiveBuff1[3]=='K')
        {
            SMS_Flag = 4;
        }
    }
    if(SMS_Flag == 4)
    {
       char Send_Buff1[30] = {0};
//        sprintf(Send_Buff1,"Po=%u         ",SMS_No_Position);
//        LcdDisplay(TOP, Send_Buff1);
//        Delay1_1s(3);
       Read_E2((char*)&Modem_setup.Retry_count,EE_Retry_Count,sizeof(Modem_setup.Retry_count));
        if( SMS_Retry_count > Modem_setup.Retry_count)
            {
                
//                SMS_Flag = 8;
//                SMS_No_Position = 4;
                Retry_Record_Update_Succeeded(SMS_Loc);
                memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
                Modem_setup.SMS_Missing_count++;
                Write_E2(EE_SMS_Missing_count,(char*)&Modem_setup.SMS_Missing_count,sizeof(Modem_setup.SMS_Missing_count));
                return;
            }
       
       Mob_Digits = Extract_SMS_Number(SMSData,Mobile_Number);
       
  

        SMS_Retry_count++;
        
        if(SMS_No_Position > 3)
        {
            goto SMS_Send_End;
        }
        else
        {
            memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
            SMS_Flag = 5;
        
            Com1StringSend("AT+CMGS=\"+",10);
            Com1StringSend(Mobile_Number,Mob_Digits);
            Com1StringSend("\"",1);
            Com1Bytesend(0x0d);
            Com1Bytesend(0x0a);
              Delay1_1s(1);
        }
       
       
      
    }
    else if(SMS_Flag == 5)
    {
        if(ReceiveBuff1[2] == '>')
       // if(!strncmp(ReceiveBuff1,">",1))
        {
            SMS_Flag = 6;
            memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
        }
        else if(!strncmp(ReceiveBuff1+2,"ERROR",5))
        {
             SMS_Flag = 0;
        }
    }
    else if(SMS_Flag == 6)
    {
          Delay1_1s(1);
        memset(Send_Buff1,' ',80);
        sprintf(Send_Buff1,"\r\nSMS Ct:%3u/%3u",Modem_setup.Per_Day_SMS_Count+1,Modem_setup.Limite_SMS);  
        Com1StringSend(SMSData+2,SMS_Length-4);
        Com1StringSend(Send_Buff1,16); // Total
        Com1Bytesend(0x1A);
        SMS_Flag = 7;    
    }
    else if((SMS_Flag == 7) && (SMS_Response_Flag == 3))
    {
        SMS_Flag = 8;
        SMS_No_Position++;
        memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
        SMS_Retry_count =0;
        Modem_setup.Per_Day_SMS_Count++;
        Write_E2(EE_Per_Day_SMS_Count,(char*)&Modem_setup.Per_Day_SMS_Count,sizeof(Modem_setup.Per_Day_SMS_Count));
    }
    else if(SMS_Flag == 8)
    {
    SMS_Send_End:
        SMS_Flag = 4;
        SMS_Response_Flag = 0;
        if(SMS_No_Position > 3)
        {
            Retry_Record_Update_Succeeded(SMS_Loc);
            SMS_Retry_count =0;
        }
        
    }
    if(SMS_Timer > 61000)
    {
        SMS_Not_Sent_Flag++;
        if(SMS_Not_Sent_Flag > 3)
        {
            LcdDisplay(TOP," SMS Sent Failed");
            Delay1_1s(1);
            SMS_Not_Sent_Flag = 0;
            Retry_Record_Update_Succeeded(SMS_Loc);
            Modem_setup.SMS_Missing_count++;
            Write_E2(EE_SMS_Missing_count,(char*)&Modem_setup.SMS_Missing_count,sizeof(Modem_setup.SMS_Missing_count));
           // Record_Update_Succeeded(SMS_Loc);
            
        }
        SMS_Timer = 0;
        GPRS_ResetFlag = 1;
        Com1Bytesend(0x1B);
        return;
    }
}

unsigned int Extract_SMS_Number(char *SMS_Data_Buff,char *Get_Mob_No)
{
    int SMS_Group = 0;
    char Mob_No_read[15] = {0};
    unsigned int Trim_Cnt,Trimmed_Length = 0;
    
Read_Next_Number:
    SMS_Group = (SMS_Data_Buff[1] - 48) - 1;        //  Extract group number to which has to sent
    Read_E2((char*)&Mob_No_read,EE_SMS_Number_1+((SMS_No_Position + (SMS_Group * 4)) * 15),sizeof(Mob_No_read));
    
    if((!strncmp(Mob_No_read,"               ",15)) || (!strncmp(Mob_No_read,"   ",3)) || (!strncmp(Mob_No_read+3,"            ",12)))
    {
        SMS_No_Position++;
        goto Read_Next_Number;
    }
    else
    {
        for(Trim_Cnt = 0;Trim_Cnt<15;Trim_Cnt++)
        {
            if((Mob_No_read[Trim_Cnt] == '0')||(Mob_No_read[Trim_Cnt] == '1')
                    ||(Mob_No_read[Trim_Cnt] == '2')||(Mob_No_read[Trim_Cnt] == '3')||(Mob_No_read[Trim_Cnt] == '4')
                    ||(Mob_No_read[Trim_Cnt] == '5')||(Mob_No_read[Trim_Cnt] == '6')||(Mob_No_read[Trim_Cnt] == '7')
                    ||(Mob_No_read[Trim_Cnt] == '8')||(Mob_No_read[Trim_Cnt] == '9'))
            {
                Get_Mob_No[Trimmed_Length] = Mob_No_read[Trim_Cnt];
                Trimmed_Length++;
            }
        }
    }
    return(Trimmed_Length);
}

