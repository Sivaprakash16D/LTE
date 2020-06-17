#define TotalNoOfPackets	2
#define MessageSize			14
#define MaxCTSinResponseToCTS	(TotalNoOfPackets-1)
#define RequestPGN			60416	// PF = 236
#define DataPGN				60160	// PF = 235

extern char CANFlag;

extern struct
{
	unsigned char MobileOptr;

}GPRS_Setup;
extern unsigned int RecordAvailable;
extern unsigned int RecordSent;
extern unsigned char GPRS_Flag;

extern void CAN_Send(void);

extern void CAN_RTRSend(void);
extern void CAN_ID(void);
extern unsigned int OutData0[4];
void RequestToSend(void);
void ClearToSendReceived(void);
void CTSReceived_GPRSSetting(char CIndex);
void FullDataACK(void);
unsigned char LSBByte(unsigned int Buff);
unsigned char MSBByte(unsigned int Buff);
unsigned int From2Bytes(char LSBByte, char MSBByte);
extern void CANDataFlow(void);
extern void ReceiveBuffID(void);
char NextPacketIndex = 0, tempVar = 0;
char IDchangeforBroadCastFlag = 0;
void SendCANdata(char SequenceNo, unsigned char Byte1, unsigned int Byte23, unsigned int Byte45, unsigned int Byte67);

void SendCANdata(char SequenceNo, unsigned char Byte1, unsigned int Byte23, unsigned int Byte45, unsigned int Byte67)
{
/*
//	IDchangeforBroadCastFlag = 1;
	CAN_ID();
//	IDchangeforBroadCastFlag = 0;
	OutData0[0] = From2Bytes(SequenceNo,Byte1);
	OutData0[1] = Byte23;
	OutData0[2] = Byte45;
	OutData0[3] = Byte67;
	CAN_Send();
*/
}

void RequestToSend(void)	//request to send
{
/*
	CAN_ID();
	OutData0[0] = From2Bytes(0x10,0x00);
	OutData0[1] = From2Bytes(0x0E,0x02);
	OutData0[2] = From2Bytes(0x01,0x00);
	OutData0[3] = From2Bytes(0xEB,0x00);
	CAN_Send();
*/
}
void ClearToSendReceived(void)	// clear to send
{
/*
	CAN_ID();
	tempVar++;
	NextPacketIndex++;
	OutData0[0] = From2Bytes(0x11,0x01);
	OutData0[1] = From2Bytes(NextPacketIndex,0xFF);
	OutData0[2] = From2Bytes(0xFF,0xFF);
	OutData0[3] = From2Bytes(0xEB,0xFF);
	CAN_Send();
*/
}
void CTSReceived_GPRSSetting(char CIndex)
{
/*
	unsigned char GPRS_Byte1,GPRS_Byte2,GPRS_Byte3,GPRS_Byte4,GPRS_Byte5,GPRS_Byte6,GPRS_Byte7,GPRS_Byte8;

	CAN_ID();
	if(CIndex == 1)								// For GPRS Mobile operator selection
	{
		OutData0[0] = From2Bytes(0x11,0xF0);
		OutData0[1] = From2Bytes(GPRS_Setup.MobileOptr,0xFF);
		OutData0[2] = From2Bytes(0xFF,0xFF);
		OutData0[3] = From2Bytes(0xEB,0xFF);
	}
	else if(CIndex == 2)						// For GPRS Display
	{
		GPRS_Byte5 = (unsigned char)(RecordAvailable >> 8) ;
		GPRS_Byte6 = (unsigned char)((RecordAvailable << 8) >> 8); 
		GPRS_Byte7 = (unsigned char)(RecordSent >> 8) ;
		GPRS_Byte8 = (unsigned char)((RecordSent << 8) >> 8); 

		OutData0[0] = From2Bytes(0x11,0xF2);
		OutData0[1] = From2Bytes(0x01,0xFF);
		OutData0[2] = From2Bytes(GPRS_Byte5,GPRS_Byte6);
		OutData0[3] = From2Bytes(GPRS_Byte7,GPRS_Byte8);
	}
	else if(CIndex == 3)								// For GPRS Mobile operator selection
	{
		OutData0[0] = From2Bytes(0x11,0xF3);
		OutData0[1] = From2Bytes(0x01,0xFF);
		OutData0[2] = From2Bytes(0xFF,0xFF);
		OutData0[3] = From2Bytes(0xEB,0xFF);
	}
	CAN_Send();
*/
}
void FullDataACK(void)
{
/*
	CAN_ID();
	tempVar++;
	OutData0[0] = From2Bytes(0x13,0x00);
	OutData0[1] = From2Bytes(0x0E,0x02);
	OutData0[2] = From2Bytes(0xFF,0x00);
	OutData0[3] = From2Bytes(0xEB,0x00);
	CAN_Send();
*/
}
unsigned char LSBByte(unsigned int Buff)
{
	unsigned char val;
//	val = ((Buff & 0xFF00) >> 8 );
	return(val);
}
unsigned char MSBByte(unsigned int Buff)
{
	unsigned char val;
//	val = ((Buff & 0x00FF));
	return(val);
}

unsigned int From2Bytes(char LSBByte, char MSBByte)	// while storing in TX Buff MSB & LSB will store correctly but while sending through can it will reverse 
{
	unsigned int Value = 0;
//	Value = (unsigned int)(MSBByte << 8) | LSBByte;
	return(Value);
}
