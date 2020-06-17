
/***************************************************************************************************************************/
/*												PAYLOAD_CON_UNIT															*/
/***************************************************************************************************************************/
#define ON   1
#define OFF  2
#define EEPROM_STARTING_ADD	0x7FF000

int LcdDispBuff[16];
char LocalDispBuff[16];
unsigned int EEROM_RAMVALUE = 0;
unsigned int EEPROMINC_VAL = 0;
unsigned char InterruptFlag = 0;

void Delay(unsigned long int);
void Initialise_Ports();
void PowerOn(void);
void UART_Config();
void Com1Bytesend(char);
void Com1StringSend(char*,char);
void Com2Bytesend(char);
void Com2StringSend(char*,char);

