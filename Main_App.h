#include "MemoryDefines.h"

#define ON   		1
#define OFF  		0
#define True 		1
#define False 		0
#define OutputOn 	0
#define OutputOff 	1

// Digital Inputs
#define IRUN		_RC2		//IP1 _RC2
#define ILATCH		_RC1		//IP2 _RC1

//Digital Outputs
#define OAMBER		1		//OP1
#define ORED		1		//OP2

//LED
#define LGREEN		_RG6			// SOLID LED 1
#define LAMBER		_RG7			// SOLID LED 2
#define LRED		_RG8			// SOLID LED 3

#define GPRS_HW_RESET   _RD10


//Baud Rate Config
#define Baud1200  	0X0185
#define Baud2400	0X00C2
#define Baud4800	0X0060
#define Baud9600	0X002F
#define Baud19200 	0X0017
#define Baud38400 	0X000B
#define Baud57600	0X0007
#define Baud115200	0X0003

#define Delay_1mS_Cnt      	7480 //((Timer1Fcy / 1000000) * 1000)
#define TIMEOUT				5000

#define GPS_ON      1
#define GPS_OFF     2
//-----------------------------------------------------//

/*********** Variables used***************/


char ReceivedChar1 = 0;					// UART
char ReceivedChar2 = 0;					// UART

char LcdDispBuff[35] = {0};				// Used for send the characters to LCD 

unsigned char InterruptFlag = 0;

// Used in Reading ADCounts
int count=0;		
int ADCOUNTS = 0;
unsigned char Samples; 
unsigned long ADCValue,ADCSampleVal;
unsigned int *ADC16Ptr;

unsigned char BytesToSend1,BytesSent1;
unsigned char BytesToSend2,BytesSent2;
#define ArraySize		150
unsigned char CharRTable2[ArraySize],PackToSend1[ArraySize];
unsigned char PackRcvd2[ArraySize],PackToSend2[ArraySize];
unsigned char CharIn1,CharIn2,CharOut1,CharOut2,CharRcvd1,CharRcvd2,CharCount1,CharCount2;
unsigned char ID,MSBCSum,LSBCSum,CSum,ByteCount1,ByteCount2,MaxChars1,MaxChars2;
unsigned long TimeInSecs;

char ReceiveBuff1[350]={0};
char ReceiveBuff2[1350]={0};
unsigned int ReceiveCounter1 = 0;
unsigned int ReceiveCounter2 = 0;

void BigDelay(unsigned char DelayValue);

unsigned int Version = 0;

unsigned char Delayval;	


unsigned char resdata[20]={1,2,5,10,20,50,100,1,2,4,8,16,32,64,0,1,2,3,4};

struct
{
	unsigned char MobileOptr;

}GPRS_Setup;

typedef struct tagFBITS {
        unsigned SendData1      :1;     
        unsigned SendData2      :1;   
        unsigned Stx1Received   :1;     
        unsigned Stx2Received   :1;     
        unsigned Etx1Received   :1;     
        unsigned Etx2Received   :1;   
        unsigned ADDATAREADY  	:1;
		unsigned OCERRFlag		:1;
		unsigned ERRFlag		:1;
		unsigned ORERRFlag1		:1;
		unsigned ORERRFlag2		:1;
		unsigned ORERRFlag3		:1;
		unsigned ORERRFlag4		:1;
		unsigned URERRFlag1		:1;
		unsigned URERRFlag2		:1;
		unsigned URERRFlag3		:1;
		unsigned URERRFlag4		:1;
		unsigned LatchedWt		:1;
		unsigned Disp_Latch_Wt	:1;

		unsigned MinsUpdate		:1;		// temp
		unsigned CLRrunTime		:1;		// temp
		unsigned ILATCH_on		:1;
		unsigned ILATCH_val		:1;
		unsigned InputCheck		:1;
		unsigned MsgDisp		:1;
		unsigned NeutralON		:1;
		unsigned RecOverWrite	:1;

		unsigned mb_req_sent	:1;
		unsigned mb_res_rxd		:1;
		unsigned ReadOperator	:1;
		unsigned ReadVehicleID	:1;
		unsigned ReadTripCount	:1;

		unsigned U1Start		:1;
		unsigned U1Ready		:1;
		unsigned U2Start		:1;


	
} FLAGBITS;
volatile FLAGBITS Flags;

struct 
{
	unsigned UART_1_Start	:1;
	unsigned UART_2_Start	:1;
	unsigned UART_1_Ready	:1;
	unsigned UART_2_Ready	:1;
	unsigned Com1Check		:1;
	unsigned Com2Check		:1;
}Bits;

// MAIN FUNCTIONS
void Init_Interrupt(void);
void InitLCDDisplay(void);

void MainScreenDisplay(void);
void Delay(unsigned long int);
void Initialise_Ports(void);
void PowerOn(void);
void UART_Config();
void Com1Bytesend(char);
void Com1StringSend(char*,int);
void Com2Bytesend(char);
void Com2StringSend(char*,int);
void ADC_Init(void);
void ReadADCCounts(unsigned char Channel);

unsigned char GetLSB(unsigned int Integer);
unsigned char GetMSB(unsigned int Integer);
void ReadSettings();

void Com1Init();
void Com2Init();
void PutDp(unsigned char*,unsigned long,char,char);

extern void WriteByteRTC(),WriteToRTC_Date(),WriteToRTC_Time();

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
    int	 Seconds;
};

extern struct struct_rtc RTC;
unsigned char RunInput= 0;	//To update the Run time
unsigned int CycCntr;
void DumpValues(void);
void Disp_mA(void);
void RunTimeUpdate(void);

unsigned char MainDisplay = 0;
unsigned int OP_Flash_Cntr = 0;

void LoadDefault(void);
extern void Delay1_1s(unsigned char);
extern void ReadRTC_DateTime(void);

unsigned long TimeOut_Cnt1 = 0;

// Menu TimeOut
unsigned long TimeOut_Cnt = 0;
struct
{
	unsigned TimeOutEnb		:1;
	unsigned TimeOut		:1; 
}TimeOutFlag;
void PortCheck(char);

//For Menu purpose
extern char GetPassword(void);
extern char GetRTCData(char,char*,char*,char*,char*);

extern unsigned char SignalLevel;
extern unsigned char GPRS_Flag;
extern unsigned int SignalTimer;

extern void GPRSInitialCheck(void);
extern void CheckSignalStrength(void);
extern void GPRS_Update(void);

extern void SendGPRSPacket(void);	//Temp
extern unsigned long GPRSTimer;

void SelMobileOperator(void);
void SIM_Balance_Check(void);
char SetOperator(char);
void ResetValues(void);
void Do_Sim_Config(void);

#define MODEM_OK	4
unsigned char Modem_Init_State = 0;
unsigned char Modem_Cmd_Sent = 0;
unsigned char Modem_Cmd_Wait = 0;
unsigned char GPRS_ResetFlag = 0;

struct GPRS
{
    unsigned SendCheck		:1;
    unsigned ModemOK		:1;
    unsigned SignalOk		:1;
    unsigned Modem_Rdy      :1;        
}GPRS_Flags1;


/*********** CAN Functions *************/

extern unsigned char LSBByte(unsigned int Buff);
extern unsigned char MSBByte(unsigned int Buff);

unsigned int Index1 = 0,Index = 0;

unsigned char GPRS_Uart1Flag = 0;

char RxBuff1[21] = {0};
char APN_Name[20] = {0};
char APNSequence = 0;
char APN_Length = 0;

unsigned char RxBuffer[100] = {0};
unsigned char RxCntr = 0;

void ServerSend(void);

unsigned long Modem_Init_Cntr = 0;
extern char TxCurrWt_Flag;
extern unsigned long TestCntr;
unsigned char SimErrorFlag = 0;
unsigned long One_Second_Counter = 0;
unsigned int DisplayLocation = 0;
void Data_Receiving(void);
unsigned long SignalLevelTimer = 0;
unsigned int Alivecounter = 0;
void Reset_LTE_Modem(void);
void Data_Sending(void);
char i=0,j=0,k=0;
char Buffer_Value[20]={0};
char Modem_Request = 0;

void Modem_Init(void);
unsigned char MemoryCheck=0;			// To check the Memory to Load Default values
unsigned int CalcLRC(char* ChecksumBuff,unsigned int ByteCount);
unsigned int LRC = 0;

unsigned int Buff_No = 0;
struct{
    unsigned int Length;
    unsigned long StartAddress;
}Buff[3];

struct{
    unsigned char InitFlag;
    unsigned char OverFlowFlag;
    unsigned int Pos;
    unsigned int Avail;
    unsigned int DisplayNo;
    unsigned long MaxRecLimit;
}Rec[3],Sent[3];

unsigned char ImmediateSentFlag = 0;
void Data_Store(unsigned int Location,char *RecData);

unsigned char ReceiveBuff[500] = {0};
unsigned int ReceiveCounter = 0;
char ImmediateSentBuff[500] = {0};
char ImmediateSentBuff1[500] = {0};

char ErrorBuff[7] = {0};
extern unsigned int Cur_Loc_Update;
void Bank_1_Data_Reset(void);
void Bank_2_Data_Reset(void);
void Bank_3_Data_Reset(void);
void Com2StringSend_with_LRC(char * LRCBuff,int Bytes);
void Testing(void);
void CalcCRC(char *SPtr,unsigned int BytesCount);
unsigned char SentDataCopied = 0;
extern unsigned int SignalLevelCheck;
void Store_Mob_Number(char *WR_Mob_Num_Buff,unsigned int BuffCnt);
void Store_Set_SMS_GPRS_Retry(char *WR_Mob_Num_Buff,unsigned int BuffCnt);
void Store_Set_SMS_Count(char *WR_Mob_Num_Buff,unsigned int BuffCnt);
void Store_Set_GPRS_LIMIT();
void Read_Mob_Number(int RD_Mob_Order);
void Read_Set_SMS_GPRS_Retry();
void Read_Set_SMS_Count();
void Read_Set_GPRS_LIMIT();
void Read_View_SMS_Count();
void Read_View_GPRS_LIMIT();
void Record_Update_Succeeded(unsigned int Loc);
void Retry_Record_Update_Succeeded(unsigned int Loc);

extern unsigned char SMS_Flag;
extern unsigned char SMS_Response_Flag;
extern unsigned long SMS_Timer;
extern unsigned int SMS_No_Position;

extern unsigned char QIACT_Flag;
unsigned char Modem_Response_Flag = 0;
unsigned char GPS_Config = 0;

void GPS_Process(void);
extern unsigned char GPS_Flag;

unsigned int GPS_Getting_Timer = 0;
unsigned int GPS_Getting_Sec = 0;
unsigned int Get_GPS_Timer = 1;

extern unsigned char Receiver_flag ;
extern char loop;
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

unsigned long typecase_String_to_long(char *Buff,unsigned int BuffCnt );

char SignalLevel_Reset = 0,GPRS_Close_flag = 0;
extern void GPRS_Close_State(void);
extern unsigned char SMS_Retry_count ;