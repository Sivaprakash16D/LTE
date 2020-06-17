#define	None 			0
#define	PtoChar			1
#define	PtoLong			2
#define	Combo			3
#define	SplFunction		4
#define	PtoInt			5  
#define ComboStr		6
#define	PtoLong1		7

#define MAXADC		2047

#define EE_MobileOptr			150
#define EE_ComMode				151
#define EE_DeviceID				152
#define EE_DestID				153


#define EE_MemoryCheck			199

extern unsigned char MemoryCheck; 
unsigned char curindex;
extern char LcdDispBuff[40];

extern unsigned long ADCSampleVal;
//extern unsigned char DeciEntry;

long minmax[18]={0,99000,4,1,99,100,100000,99,500,1500,255,50000,9,30,65534,99999,200,999};

char codedpt[7]={0,1,2,3,4,5,6};

long cmbminmax[7]={0,6,8,13,14,18,2};	//V2.3 - Gain 1 is disabled
unsigned char cmbdata[20]={1,2,5,10,20,50,100,1,2,4,8,16,32,64,0,1,2,3,4};
long cmbstrminmax[17]={0,4,5,7,8,10,11,13,14,15,11,12,16,18,6};
unsigned char cmbdatatostr[20] = {0,1,2,3,4,0,1,2,0,1,2,0,1,2,0,1,0,1,2};
unsigned char cmbstring[19][17] = {"            none","              gm","              Kg","             Ton","              lb","         REQUEST","            DUMP"," PROTOCOL-2 DUMP","        Both OFF","         Both ON","      Last Stage","       Low Value","      High Value","      Last Value","         Disable","         Enable ","             LOW","          MEDIUM","            HIGH"};
char unitname[5][5] = {"None","  gm","  Kg"," Ton","  lb"};
char unitprint[5][4] = {"   ","gm ","Kg ","Ton","lb "};
unsigned int Sflfun[26]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};

extern struct{
	int RAWADC;
	int TripCount;	
	unsigned long CumulativeWeight;
	unsigned long MaxCapacity;
	unsigned long CalCapacity;
	int Zero;
	int Zero1;
	int Span;	
	unsigned char Resol;
	long EnggWt;
	long LatchedWt;

}Calibration;

extern struct
{
	unsigned long REFmA1;
	unsigned long REFmA2;
	int REF1Cnt;
	int REF2Cnt;
	int Ch1Ref1;
	int Ch1Ref2;
//	int Ch1RefmA;
	int Ch2Ref1;
	int Ch2Ref2;
//	int Ch2R2mA;
	int Ch3Ref1;
	int Ch3Ref2;
//	int Ch3R2mA;
	int Ch4Ref1;
	int Ch4Ref2;
//	int Ch4R2mA;
	int ChxRef_mA;
	int Offset_mA;

}CurrentCalib;

extern struct
{
	unsigned long MinWt;
	unsigned long LatchDelay;
	unsigned char Channel1;
	unsigned char Channel2;
	unsigned char Channel3;
	unsigned char Channel4;
	unsigned long OverWeight;
	unsigned long Password;
	unsigned long TruckSerial;
	unsigned long SetPoint1;
	unsigned long SetPoint2;
	unsigned long SetPoint3;
	unsigned char InputLogic;
	unsigned char PLMS_Status;
	unsigned char DumpDisplay;	//Upto this not used

	unsigned char DeviceID;
	unsigned char DestID; 
	unsigned char ComMode;
}Setup;

extern struct
{
	unsigned char MobileOptr;

}GPRS_Setup;

extern struct
{
	char Date;
	char Month;
	int Year;
	char Hour;
	char Minute;
	char Second;
	char TempPad;
	unsigned long Weight;
	unsigned char OLStatus;
	unsigned char TempFlag;
	
}TripRecord;


extern char CurrentMin;
char RunTimeClr = 0;

struct DataRegis
{
	union
	{
		float          fData;
		unsigned long  ulData;
		long           lData;
		unsigned long *pulData;
		long          *plData;
		unsigned int   uiData;
		int            iData;
		unsigned int   *puiData;
		int            *piData;
		unsigned char  ucData;
		signed char    scData;
		unsigned char *pucData;
		signed char   *pscData;
	}Val;
};

struct __MENUDESCR
{
   
	unsigned int Next;  
	unsigned int Enter;   
	unsigned int Esc;
	unsigned int E2Index;
	char Name[17];

	union
	{
	  unsigned char Value;
		
	  struct
	  {
		 unsigned  char WriteE2:1;    //e2 prom writing wanted
		 unsigned  char Validation:1; // validation reqd
		 unsigned  char Blink:1;	  // blink reqd while getting the values
		 unsigned  char SeeMin:1;	  // see min value for validation
		 unsigned  char SeeMax:1;	  // see max value for validation
		 unsigned  char Numeric:1;	  // Input from user thro the keypads
		 unsigned  char Sign:1;		  // sign to get as input
		 unsigned  char isPassword:1; // if blink flag then blinking
	  }flags;
	}mnuFlags;
	unsigned char mnuType;

	struct DataRegis *Value;
	
	long *Min;
	long *Max;

	unsigned int mnuCurIndex; // current index
	unsigned char CurScanDigiVal;
	unsigned char NumDigit:4;//to go till how much digit for scanning
	unsigned char CurDigi:4;//used for cur index
	unsigned char ShowHead:4;//alignment 0,1,2,3
	unsigned char ShowValue:4;//alignment 0,1,2,3
	unsigned char *NumDecimal;
	unsigned char ErrNo;
};

#define MaxOption 	50

const struct __MENUDESCR MenuData[MaxOption] =
{
{1,1,1,0, "*     MENU     *",0x00,0,0,0,0,0,0,0,0,0,0,0,0},//0

{2,1,0,EE_MobileOptr,"*  MOBILE OPR  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[18],0,0,1,0,0,0,0,0,0,0},//1
{3,2,0,EE_ComMode,"*  COM MODE    *",0x20,SplFunction,(struct DataRegis*)&Sflfun[20],0,0,2,0,0,0,0,0,0,0},//2
{4,3,0,EE_DeviceID,"*  Source ID   *",0x20,SplFunction,(struct DataRegis*)&Sflfun[21],0,0,3,0,0,0,0,0,0,0},//3
{1,4,0,EE_DestID,"*   Dest. ID   *",0x20,SplFunction,(struct DataRegis*)&Sflfun[22],0,0,4,0,0,0,0,0,0,0},//4

{6,5,1,0,"*  MOBILE OPR  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[18],0,0,5,0,0,0,0,0,0,0},//5 
{7,6,1,0,"*  BAL CHECK   *",0x20,SplFunction,(struct DataRegis*)&Sflfun[17],0,0,6,0,0,0,0,0,0,0},//6
{8,7,1,0,"* RTC-SET DATE *",0x20,SplFunction,(struct DataRegis*)&Sflfun[9],0,0,7,0,0,0,0,0,0,0},//7
{5,8,1,0,"* RTC-SET TIME *",0x20,SplFunction,(struct DataRegis*)&Sflfun[10],0,0,8,0,0,0,0,0,0,0},//8
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,9,0,0,0,0,0,0,0},//9
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,10,0,0,0,0,0,0,0},//10
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,11,0,0,0,0,0,0,0},//11
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,12,0,0,0,0,0,0,0},//12

{14,13,2,0,"*    KEYPAD    *",0x20,SplFunction,(struct DataRegis*)&Sflfun[6],0,0,13,0,0,0,0,0,0,0},//13
{15,14,2,0,"*    DISPLAY   *",0x20,SplFunction,(struct DataRegis*)&Sflfun[14],0,0,14,0,0,0,0,0,0,0},//14
{16,15,2,0,"*    COM-1     *",0x20,SplFunction,(struct DataRegis*)&Sflfun[4],0,0,15,0,0,0,0,0,0,0},//15
{17,16,2,0,"*    COM-2     *",0x20,SplFunction,(struct DataRegis*)&Sflfun[5],0,0,16,0,0,0,0,0,0,0},//16
{18,17,2,0,"*   RTC-CHECK  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[15],0,0,17,0,0,0,0,0,0,0},//17
{13,18,2,0,"*   RTC-INIT.  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[11],0,0,18,0,0,0,0,0,0,0},//18

/*
{2,5,0,0,"*    SET UP    *",0x00,0,0,0,0,1,0,0,0,0,0,0,0},//1
{1,13,0,0,"*  DIAGNOSTICS *",0x00,0,0,0,0,2,0,0,0,0,0,0,0},//2
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,3,0,0,0,0,0,0,0},//3
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,4,0,0,0,0,0,0,0},//4

{6,5,1,0,"*  MOBILE OPR  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[18],0,0,5,0,0,0,0,0,0,0},//5 
{7,6,1,0,"*  BAL CHECK   *",0x20,SplFunction,(struct DataRegis*)&Sflfun[17],0,0,6,0,0,0,0,0,0,0},//6
{8,7,1,0,"* RTC-SET DATE *",0x20,SplFunction,(struct DataRegis*)&Sflfun[9],0,0,7,0,0,0,0,0,0,0},//7
{5,8,1,0,"* RTC-SET TIME *",0x20,SplFunction,(struct DataRegis*)&Sflfun[10],0,0,8,0,0,0,0,0,0,0},//8
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,9,0,0,0,0,0,0,0},//9
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,10,0,0,0,0,0,0,0},//10
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,11,0,0,0,0,0,0,0},//11
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,12,0,0,0,0,0,0,0},//12

{14,13,2,0,"*    KEYPAD    *",0x20,SplFunction,(struct DataRegis*)&Sflfun[6],0,0,13,0,0,0,0,0,0,0},//13
{15,14,2,0,"*    DISPLAY   *",0x20,SplFunction,(struct DataRegis*)&Sflfun[14],0,0,14,0,0,0,0,0,0,0},//14
{16,15,2,0,"*    COM-1     *",0x20,SplFunction,(struct DataRegis*)&Sflfun[4],0,0,15,0,0,0,0,0,0,0},//15
{17,16,2,0,"*    COM-2     *",0x20,SplFunction,(struct DataRegis*)&Sflfun[5],0,0,16,0,0,0,0,0,0,0},//16
{18,17,2,0,"*   RTC-CHECK  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[15],0,0,17,0,0,0,0,0,0,0},//17
{13,18,2,0,"*   RTC-INIT.  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[11],0,0,18,0,0,0,0,0,0,0},//18

*/
/*
{4,2,0,0, "* CURRENT CALIB*",0x00,0,0,0,0,1,0,0,0,0,0,0,0},//1
{3,2,1,0, "*  REFERENCE 1 *",0x23,PtoLong,(struct DataRegis*)&CurrentCalib.REFmA1,&minmax[3],&minmax[17],2,0,3,0,0,0,&codedpt[1],0},//2
{2,3,1,0, "*  REFERENCE 2 *",0x23,PtoLong,(struct DataRegis*)&CurrentCalib.REFmA2,&minmax[3],&minmax[17],3,0,3,0,0,0,&codedpt[1],0},//3

{12,6,0,0, "* LOAD CAL AUTO*",0x00,0,0,0,0,4,0,0,0,0,0,0,0},//4
{6,6,4,0,"*DECIMAL POINT *",0x23,PtoLong,0,&minmax[0],&minmax[2],5,0,1,0,0,0,&codedpt[0],0},//5
{7,7,4,0, "* MAX-CAPACITY *",0x23,PtoLong,(struct DataRegis*)&Calibration.MaxCapacity,&minmax[3],&minmax[1],6,0,5,0,0,0,&codedpt[1],0},//6
{8,8,4,0,  "*  RESOLUTION  *",0x21,Combo,(struct DataRegis*)&Calibration.Resol,&cmbminmax[0],&cmbminmax[6],7,0,0,0,0,0,&cmbdata[0],0},//7
{9,9,4,0,"*   CAL ZERO   *",0x20,SplFunction,(struct DataRegis*)&Sflfun[0],0,0,8,0,0,0,0,0,0,0},//8
{10,10,4,0,"*   CAL SPAN   *",0x20,SplFunction,(struct DataRegis*)&Sflfun[1],0,0,9,0,0,0,0,0,0,0},//9
{11,11,4,0, "* CAL-CAPACITY *",0x23,PtoLong,(struct DataRegis*)&Calibration.CalCapacity,&minmax[3],&Calibration.MaxCapacity,10,0,5,0,0,0,&codedpt[1],0},//10
{6,11,4,0,"*    CAL END   *",0x00,0,0,0,0,11,0,0,0,0,0,0,0},//11

{15,6,0,0, "* LOAD CAL MANL*",0x00,0,0,0,0,12,0,0,0,0,0,0,0},//12
{14,14,12,0,"*   CAL ZERO   *",0x23,PtoInt,(struct DataRegis*)&Calibration.Zero,&minmax[3],&minmax[14],13,0,4,0,0,0,0,0},//13
{10,10,12,0,"*   CAL SPAN   *",0x23,PtoInt,(struct DataRegis*)&Calibration.Span,&minmax[3],&minmax[14],14,0,4,0,0,0,0,0},//14

{31,16,0,0,"*    SET UP    *",0x00,0,0,0,0,15,0,0,0,0,0,0,0},//15
{17,17,15,0,"*    MIN.WT    *",0x23,PtoLong,(struct DataRegis*)&Setup.MinWt,&minmax[0],&Setup.OverWeight,16,0,5,0,0,0,&codedpt[1],0},//16
{18,18,15,0,"*   OVER LOAD  *",0x23,PtoLong,(struct DataRegis*)&Setup.OverWeight,&Setup.MinWt,&Calibration.MaxCapacity,17,0,5,0,0,0,&codedpt[1],0},//17
{19,19,15,0,"*   RED GLOW   *",0x23,PtoLong,(struct DataRegis*)&Setup.SetPoint3,&Setup.SetPoint2,&Calibration.MaxCapacity,18,0,5,0,0,0,&codedpt[1],0},//18
{20,20,15,0,"*  AMBER FLASH *",0x23,PtoLong,(struct DataRegis*)&Setup.SetPoint2,&Setup.SetPoint1,&Setup.SetPoint3,19,0,5,0,0,0,&codedpt[1],0},//19
{22,22,15,0,"*  AMBER GLOW  *",0x23,PtoLong,(struct DataRegis*)&Setup.SetPoint1,&minmax[0],&Setup.SetPoint2,20,0,5,0,0,0,&codedpt[1],0},//20
{22,22,15,0,"*  CH. CONFIG  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[8],0,0,21,0,0,0,0,0,0,0},//21
{23,23,15,0,"* INPUT LOGIC  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[16],0,0,22,0,0,0,0,0,0,0},//22
{24,24,15,0,"* RTC-SET DATE *",0x20,SplFunction,(struct DataRegis*)&Sflfun[9],0,0,23,0,0,0,0,0,0,0},//23
{25,25,15,0,"* RTC-SET TIME *",0x20,SplFunction,(struct DataRegis*)&Sflfun[10],0,0,24,0,0,0,0,0,0,0},//24
{26,26,15,0,"*  LATCH DELAY *",0x23,PtoLong,(struct DataRegis*)&Setup.LatchDelay,&minmax[3],&minmax[4],25,0,2,0,0,0,0,0},//25
{27,27,15,0,"* TRUCK SERIAL *",0x23,PtoLong,(struct DataRegis*)&Setup.TruckSerial,&minmax[3],&minmax[15],26,0,5,0,0,0,0,0},//26
{28,28,15,0,"* DUMP DISPLAY *",0x20,SplFunction,(struct DataRegis*)&Sflfun[17],0,0,27,0,0,0,0,0,0,0},//27


{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,29,0,0,0,0,0,0,0},//29
{0,0,0,0,"*     SPARE    *",0x00,0,0,0,0,30,0,0,0,0,0,0,0},//30

{1,32,0,0,"*  DIAGNOSTICS *",0x00,0,0,0,0,31,0,0,0,0,0,0,0},//31
{33,32,31,0,"*    RAW ADC   *",0x20,SplFunction,(struct DataRegis*)&Sflfun[2],0,0,32,0,0,0,0,0,0,0},//32
{34,33,31,0,"*    KEYPAD    *",0x20,SplFunction,(struct DataRegis*)&Sflfun[6],0,0,33,0,0,0,0,0,0,0},//33
{35,34,31,0,"*    DISPLAY   *",0x20,SplFunction,(struct DataRegis*)&Sflfun[14],0,0,34,0,0,0,0,0,0,0},//34
{36,35,31,0,"*  DIGITAL I/P *",0x20,SplFunction,(struct DataRegis*)&Sflfun[7],0,0,35,0,0,0,0,0,0,0},//35
{37,36,31,0,"*  DIGITAL O/P *",0x20,SplFunction,(struct DataRegis*)&Sflfun[13],0,0,36,0,0,0,0,0,0,0},//36
{39,37,31,0,"*     COMM     *",0x20,SplFunction,(struct DataRegis*)&Sflfun[5],0,0,37,0,0,0,0,0,0,0},//37
{34,38,31,0,"*   RTC-CHECK  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[15],0,0,38,0,0,0,0,0,0,0},//38 Not Used
{40,39,31,0,"*   RTC-INIT.  *",0x20,SplFunction,(struct DataRegis*)&Sflfun[11],0,0,39,0,0,0,0,0,0,0},//39
{41,40,31,0,"* RUNTIME-INIT *",0x20,SplFunction,(struct DataRegis*)&Sflfun[19],0,0,40,0,0,0,0,0,0,0},//40
{32,41,31,0,"* LOAD DEFAULT *",0x20,SplFunction,(struct DataRegis*)&Sflfun[12],0,0,41,0,0,0,0,0,0,0},//41
*/

};

extern struct 
{
	unsigned UART_1_Start	:1;
	unsigned UART_2_Start	:1;
	unsigned UART_1_Ready	:1;
	unsigned UART_2_Ready	:1;
	unsigned Calib_Manl		:1;
	unsigned Com1Check		:1;
	unsigned Com2Check		:1;

}Bits;




void DoMenu(void);
char GetCombo(char);
char GetData(char*,char,char,char,long,long,char);
void ShowAD(char);
void KeypadTest(void);
void DIPCheck(void);
extern void PortCheck(char);
void ChannelConfigSel(void);
void ChannelConfiguration(void);
void ChannelSelect(unsigned char);
void DOPCheck(void);
void DisplayCheck(void);
void RTC_Check(void);
void RTC_Init(void);
void Set_Date(void);
void Set_Time(void);
char GetRTCData(char,char*,char*,char*,char*);
char GetPassword(void);
extern void LoadDefault(void);
char Getvalues_mA(char,long);
char Show_mA(void);
int Calf_mA(int,int,int,int);
void ILSelect(void);
//void HeaderSelect(void);
//void TruckModelSelect(void);
void DumpDisplaySel(void);
void DumperRunTime(void);
void ClearRunTime(void);


extern long RoundOff(long value,char round);

extern void ReadADCCounts(unsigned char Channel);
extern void CalcADC(void);
extern void Delay1_1s(unsigned char);
extern void Delay(unsigned long int);
extern void ChannelConfigSel(void);
extern void RunTimeUpdate(void);
extern void ReadRTC_DateTime(void);
extern void PutDp(unsigned char*,unsigned long,char,char);

extern unsigned char RunInput;	//To update the Run time

extern unsigned char MainDisplay;
unsigned char PasswordEntry = 0;

unsigned char TimeFormat24hrMax[7]={"235959"};
unsigned char TimeFormat24hrMin[7]={"000000"};
unsigned char TimeFormat24hrComp[7]={"N2N0N0"};

unsigned char DateFormatMax[7]={"311299"};
unsigned char DateFormatMin[7]={"010100"};
unsigned char DateFormatComp[7]={"N3N1N0"};


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


//Menu Timeout
extern unsigned long TimeOut_Cnt;
extern struct
{
	unsigned TimeOutEnb		:1;
	unsigned TimeOut		:1; 
}TimeOutFlag;

// For GPRS
extern char ReceiveBuff[150];
extern unsigned int ReceiveCounter;

extern void SelMobileOperator(void);
extern char SetOperator(char);

void SetDeviceID(void);
void SetDestinationID(void);
void SetComMode(void);




