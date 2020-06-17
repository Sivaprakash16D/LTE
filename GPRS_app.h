#include "MemoryDefines.h"

#define TOP         1
#define BOTTOM      2
#define MODEM_OK	0xFF

unsigned long GPRSTimer = 0;

unsigned int SignalTimer = 0;
unsigned char SignalLevel = 0;
unsigned char GPRS_Flag = 0;
char Send_Buff1[130] = {0};

extern char ReceiveBuff1[350];
extern unsigned char Modem_Init_State;
extern unsigned int ReceiveCounter1;

void GPRSInitialCheck(void);
void CheckSignalStrength(void);
void GPRS_Update(void);

extern struct GPRS
{
    unsigned SendCheck		:1;
    unsigned ModemOK		:1;
    unsigned SignalOk		:1;
    unsigned Modem_Rdy      :1;
}GPRS_Flags1;

extern void Write_E2(unsigned long,char*,int);
extern void Read_E2(char*,unsigned long,int);

extern void Delay1_1s(unsigned char);
extern void Delay(unsigned long int);

extern void Com1Bytesend(char);
extern void Com1StringSend(char*,int);
extern void Com2Bytesend(char);
extern void Com2StringSend(char*,int);

extern void LcdDisplay(char Mode,char *Buff);
extern char LcdDispBuff[35];

extern void PutDp(unsigned char*,unsigned long,char,char);
extern unsigned char GPRS_ResetFlag;
extern unsigned char GPRS_Uart1Flag;

extern unsigned char CurrentWtRxFlag;
unsigned long TestCntr = 0;

extern char Modem_Request;
extern unsigned char ImmediateSentFlag;

extern struct{
    unsigned int Length;
    unsigned long StartAddress;
}Buff[4];

extern struct{
    unsigned char InitFlag;
    unsigned char OverFlowFlag;
    unsigned int Pos;
    unsigned int Avail;
    unsigned int DisplayNo;
    unsigned long MaxRecLimit;
}Rec[3],Sent[3];

extern char ImmediateSentBuff[500],ImmediateSentBuff1[500];
extern void Data_Update(unsigned int Loc,char *SendData,unsigned int SendData_Length);
unsigned int Cur_Loc_Update = 0;
extern void Com2StringSend_with_LRC(char * LRCBuff,int Bytes);
extern unsigned char SentDataCopied;
unsigned int SignalLevelCheck = 0;
extern void Send_SMS(unsigned int Loc,char *SendData,unsigned int SendData_Length);
extern void Record_Update_Succeeded(unsigned int Loc);
extern void Retry_Record_Update_Succeeded(unsigned int Loc);
extern unsigned char Temp_Error_DispFlag;


unsigned char QIACT_Flag = 0,Receiver_flag =0,Reset_count =0;
struct 
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
unsigned char GPRS_Retry_count =0;

 int Reset_flag =0,Reset_flag1 = 0;

void GPRS_Close_State(void);
extern char GPRS_Close_flag;
extern char ReceiveBuff2[1350];
extern unsigned int ReceiveCounter2 ;
extern unsigned char SMS_Flag ;