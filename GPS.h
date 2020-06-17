#define GPS_ON      1
#define GPS_OFF     2

#define TOP 1
#define BOTTOM 2

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

extern unsigned char GPS_Config;
unsigned char GPS_Flag = 0;

extern unsigned int GPS_Getting_Timer;
extern unsigned int GPS_Getting_Sec;
extern unsigned int Get_GPS_Timer;


