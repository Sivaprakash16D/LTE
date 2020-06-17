#define TOP 1
#define BOTTOM 2

void Send_SMS(unsigned int Loc,char *SendData,unsigned int SendData_Length);

unsigned char SMS_Flag = 0;
unsigned char SMS_Response_Flag = 0;

extern char ReceiveBuff1[350];
extern unsigned int ReceiveCounter1;

extern void Com1Bytesend(char);
extern void Com1StringSend(char*,int);
extern void Com2Bytesend(char);
extern void Com2StringSend(char*,int);

extern void LcdDisplay(char Mode,char *Buff);
extern char LcdDispBuff[35];

extern void Delay1_1s(unsigned char);

extern void Write_E2(unsigned long,char*,int);
extern void Read_E2(char*,unsigned long,int);

extern struct{
    unsigned char InitFlag;
    unsigned char OverFlowFlag;
    unsigned int Pos;
    unsigned int Avail;
    unsigned int DisplayNo;
    unsigned long MaxRecLimit;
}Rec[3],Sent[3];

extern void Record_Update_Succeeded(unsigned int Loc);
extern void Retry_Record_Update_Succeeded(unsigned int Loc);
unsigned long SMS_Timer = 0;
extern unsigned char GPRS_ResetFlag;

unsigned int Extract_SMS_Number(char *SMS_Data_Buff,char *Get_Mob_No);
unsigned int SMS_No_Position = 0;
unsigned char SMS_Not_Sent_Flag = 0;

extern struct 
{
    
  
    unsigned char Retry_count;
    unsigned char RTC_Date_Temp;
    unsigned int RTC_Year_Temp;
    unsigned int Limite_SMS;
    unsigned int Per_Day_SMS_Count;
    unsigned int SMS_Missing_count;
    unsigned long Limite_Server_data_usage;
    unsigned long Per_Day_Server_data_usage;
    unsigned long Missing_Server_data;

}Modem_setup;

extern char Send_Buff1[130] ;
unsigned char SMS_Retry_count =0;
