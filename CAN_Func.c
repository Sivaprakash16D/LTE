#include "p30fxxxx.h"


#define FCY 		30000000             		// 30 MHz
#define BITRATE 	1000000			 			// 1Mbps
#define NTQ 		15							// Number of Tq cycles which will make the 

//#define EEPROM_STORING_ADD		211	
#define SIZE_REC	29

#define EE_SerialNo				10
#define EE_OperatorID			46
#define EE_OperatorName			54
#define EE_RecordSent			80
#define EE_RecordAvail			84
#define EE_TotalTrip			88
#define EE_RecordLoopBack		92

#define	EE_MobileOptr			150
#define EE_ComMode				151
#define EE_DeviceID				152
#define EE_DestID				153

#define EE_RxCurrentWeight		154
#define EE_RxSetWeight			158
#define EE_RxMasterID			162
#define EE_RxTruckSerial		164
			
#define EE_DATA_ERR				200
#define EE_DATA_ADD				400

#define EEAPN_Length			100	//42300
#define EEAPN_Name				102

#define MAXRECORDS 				3000 	// Record Storage Limit 3675	3000


//CAN Bit Timing 
#define BRP_VAL		((FCY/(2*NTQ*BITRATE))-1)  //Formulae used for C1CFG1bits.BRP // For 250Kbps

#define CAN1 _LATG13 		// STB-SELECT1 line TJA1040
//#define CAN2 _LATG14		// STB-SELECT2 line TJA1040

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

extern void ResetValues(void);
void GPRS_Setting(void);
void GPRS_Setting_BroadCast_254(void);unsigned int TempRxSID_GS,TempRxEID_GS,TempRxDLC_GS;

extern struct OPRecord 
{
	char Operator[8];
	char Name[20];
}Optr;

void Can_Init(void);
void CAN_Send(void);
void CAN_RTRSend(void);
void CAN_ID(void);
void ReceiveBuffID(void);
void CANDataFlow(void);

extern void Write_E2(unsigned long,char*,int);
extern void Read_E2(char*,unsigned long,int);
//---------------------------------------------------------------------
unsigned int OutData0[4] = {0x3030, 0x3030, 0x3030, 0x3030};             
unsigned int OutData1[2] = {0x0000, 0x0000};
unsigned int OutData2[4] = {0x0000, 0x0000, 0x0000, 0x0000};            
unsigned int OutData3[2] = {0x0000, 0x0000};
unsigned int TempRxSID = 0,TempRxEID = 0,TempRxDLC = 0;
// Intilializing the receive registers to be 0

unsigned int InData0[4] = {0, 0, 0, 0};
unsigned int InData1[4] = {0, 0, 0, 0};
unsigned int InData2[4] = {0, 0, 0, 0};
unsigned int InData3[4] = {0, 0, 0, 0};

unsigned char Buff1[2]={0,0};
unsigned char Buff2[2]={0,0};
unsigned char Buff3[2]={0,0};
unsigned char Buff4[2]={0,0};

unsigned char C1T0_flag = 0;
unsigned char C1T1_flag = 0;
unsigned char C1R0_flag = 0;
unsigned char C1R1_flag = 0; 
unsigned char C1ER_flag = 0;

unsigned char C2T0_flag = 0;
unsigned char C2T1_flag = 0;
unsigned char C2R0_flag = 0;
unsigned char C2R1_flag = 0; 
unsigned char C2ER_flag = 0;

/* ERROR FLAGS */
unsigned char  C1IVR_flag = 0;
unsigned char  C1EWARN_flag = 0;
unsigned char  C1RXWAR_flag = 0;
unsigned char  C1TXWAR_flag = 0;
unsigned char  C1RXEP_flag = 0;
unsigned char  C1TXEP_flag = 0;
unsigned char  C1TXBO_flag = 0;
unsigned char RTR_flag = 0;

unsigned char  C2IVR_flag = 0;
unsigned char  C2EWARN_flag = 0;
unsigned char  C2RXWAR_flag = 0;
unsigned char  C2TXWAR_flag = 0;
unsigned char  C2RXEP_flag = 0;
unsigned char  C2TXEP_flag = 0;
unsigned char  C2TXBO_flag = 0;

char GPRSBroadcast_Flag = 0;
unsigned long i = 0,j = 0;
extern char LcdDispBuff[40];
extern void SendCANdata(char SequenceNo, unsigned char Byte1, unsigned int Byte23, unsigned int Byte45, unsigned int Byte67);
extern char CANFlag;
extern unsigned int RecordsUpdated;
extern char IDchangeforBroadCastFlag;
extern unsigned char RecordOverflow_Flag;
extern unsigned char RecordUpdateOver_Flag;
extern unsigned char RecordLoopBack_Flag;
extern unsigned int RecordSent;
extern unsigned char SignalLevel;
extern unsigned char GPRS_Flag;
extern unsigned char Modem_Init_State;
char FrameSequenceNumber = 0;
char TotalPackets = 0;
extern char NextPacketIndex;
static unsigned char Byte1,Byte2,Byte3,Byte4,Byte5,Byte6,Byte7,Byte8;
unsigned int TempRxSID_G = 0,TempRxEID_G = 0,TempRxDLC_G = 0;
extern unsigned long SerialNumber;
char PowerOnFlag = 0;
unsigned char TempFlag = 0;
extern char IDchangeforBroadCastFlag;
extern unsigned int RecordAvailable;
extern void Delay1_1s(unsigned char);
extern unsigned int TotalTripCount;

void ReceiveBuffID_GPRSReq(void);

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
extern struct Vehicle
{
	char SerialNo[8];
	char ModelName[8];
	char Customer[20];
}VehicleID;
struct
{
	unsigned Priority : 3;
	unsigned Reserved : 1;
	unsigned DataPage : 1;
	unsigned PF		  :	8;
	unsigned PS		  : 8;
	unsigned SAddress : 8;
}CANIdentifier_29,CANRecIdentifier_29;
void ReceiveBuffID_GPRSReq(void)
{
	CANRecIdentifier_29.Priority = 6;	// constant
	CANRecIdentifier_29.Reserved = 0;	// constant
	CANRecIdentifier_29.DataPage = 0;	// constant
	CANRecIdentifier_29.PF = 236;
	CANRecIdentifier_29.PS = 255;
	CANRecIdentifier_29.SAddress = Setup.DestID;
	TempRxSID_G = (CANRecIdentifier_29.Priority << 10) | ((CANRecIdentifier_29.PF >> 2) << 2) | 0x01; 
//	TempRxSID = 0x0001 1000 1110 1101 
//	TempRxEID = 0x0000 0000 0000 0100
//	TempRxDLC = 0x0000 1000 0000 1000;
	TempRxEID_G = ((CANRecIdentifier_29.PF & 0x03) << 10) | (CANRecIdentifier_29.PS << 2) |((CANRecIdentifier_29.SAddress & 0xC0) >> 6);
	TempRxDLC_G = ((CANRecIdentifier_29.SAddress & 0x3F) << 10) | (0x0118);
}
void ReceiveBuffID(void)
{
	CANRecIdentifier_29.Priority = 6;	// constant
	CANRecIdentifier_29.Reserved = 0;	// constant
	CANRecIdentifier_29.DataPage = 0;	// constant
	CANRecIdentifier_29.PF = 236;
	CANRecIdentifier_29.PS = Setup.DeviceID;
	CANRecIdentifier_29.SAddress = Setup.DestID;
	TempRxSID = (CANRecIdentifier_29.Priority << 10) | ((CANRecIdentifier_29.PF >> 2) << 2) | 0x01; 
//	TempRxSID = 0x0001 1000 1110 1101 
//	TempRxEID = 0x0000 0000 0000 0100
//	TempRxDLC = 0x0000 1000 0000 1000;
	TempRxEID = ((CANRecIdentifier_29.PF & 0x03) << 10) | (CANRecIdentifier_29.PS << 2) |((CANRecIdentifier_29.SAddress & 0xC0) >> 6);
	TempRxDLC = ((CANRecIdentifier_29.SAddress & 0x3F) << 10) | (0x0118);
}
void CAN_ID(void)
{
	CANIdentifier_29.Priority = 6;	// constant
	CANIdentifier_29.Reserved = 0;	// constant
	CANIdentifier_29.DataPage = 0;	// constant
	CANIdentifier_29.PF = 236;
	if(IDchangeforBroadCastFlag == 1)
		CANIdentifier_29.PS = 255;
	else if(GPRSBroadcast_Flag == 1)
		CANIdentifier_29.PS = 254;
	else
		CANIdentifier_29.PS = Setup.DestID;

	CANIdentifier_29.SAddress = Setup.DeviceID;
/*
	C1TX0SID = 0xC0ED;     // SID - 0x222  // extended identifier 
	C1TX0EID = 0x0008;     // EID 
	C1TX0DLC = 0x05C0;		//Select the Data word Length for CAN1 Transmit Buffer0 which is 8 byte  
*/

	C1TX0SIDbits.SID10_6 = (CANIdentifier_29.Priority << 2) | (CANIdentifier_29.Reserved << 1)| CANIdentifier_29.DataPage;
	C1TX0SIDbits.SID5_0 = (CANIdentifier_29.PF >> 2);
	C1TX0EIDbits.EID17_14 = ((CANIdentifier_29.PF << 6) >> 4) | (CANIdentifier_29.PS >> 6);
	C1TX0EIDbits.EID13_6 = (CANIdentifier_29.PS << 2)| (CANIdentifier_29.SAddress >> 6);
	C1TX0DLCbits.EID5_0 = (CANIdentifier_29.SAddress << 2 ) >> 2;
	
	C1TX0SIDbits.TXIDE = 1;	// enable extended ID
	C1TX0SIDbits.SRR = 0;	// Not used in EX ID
	C1TX0DLCbits.DLC = 8;	// data length 8	
	C1TX0DLCbits.TXRTR = 0;	// normal message ( no RTR)
//	C1TX0SID |= 0x0003;
//	C1TX0DLC &= 0xFEEF;
}
void CAN_RTRSend(void)
{
	if(C1T0_flag == 1)
	{
		C1T0_flag = 0; 
		C1TX0B1 = OutData0[0]; 
	  	C1TX0B2 = OutData0[1]; 
	 	C1TX0B3 = OutData0[2]; 
	 	C1TX0B4 = OutData0[3]; 

		C1TX0CONbits.TXREQ = 1;
		C1TX0SIDbits.TXIDE = 1;
		C1TX0SIDbits.SRR = 0;
		C1TX0DLCbits.TXRTR = 1;
		C1TX0DLCbits.DLC = 8;
 
	}
}
void CAN_Send()
{

		if(C1T0_flag == 1)
		{
			C1T0_flag = 0; 
 
			C1TX0B1 = OutData0[0]; 
		  	C1TX0B2 = OutData0[1]; 
		 	C1TX0B3 = OutData0[2]; 
		 	C1TX0B4 = OutData0[3]; 
			
			C1TX0CONbits.TXREQ = 1;
			C1TX0SIDbits.TXIDE = 1;
			C1TX0SIDbits.SRR = 0;
			C1TX0DLCbits.DLC = 8;
			C1TX0DLCbits.TXRTR = 0;

		}
		while(C1TX0CONbits.TXREQ == 1){
//		LcdDisplay(BOTTOM,"  CANERROR...   ");
		if(C1ER_flag == 1)
		{
//			C1TX0CONbits.TXREQ = 0;
			Can_Init();
/*
			while(C1INTFbits.TXBO ==1)
			{
				Can_Init();
				LcdDisplay(2,"0000067890123456");
			}
			while(C1INTFbits.TXEP ==1)
			{
				Can_Init();
				LcdDisplay(2,"1111167890123456");
			}

			while(C1INTFbits.RXEP ==1)
			{
				Can_Init();
				LcdDisplay(2,"2222267890123456");
			}
*/
//			CANErrorFlag = 1;
//			CANCTR++;
//			CANFlag = 0;
			C1ER_flag = 0;
			LcdDisplay(2,"1234567890123456");
		}
		}
		
}
void CANDataFlow(void)
{
 	unsigned int RecordsUpdateLocation = 0;	
	char PrintBuff[40];
	if(CANFlag == 1)
	{
		ReceiveBuffID();
		ReceiveBuffID_GPRSReq();
		if((C1RX0SID == TempRxSID_G && C1RX0EID == TempRxEID_G && C1RX0DLC == TempRxDLC_G && C1R0_flag == 1)|| (C1RX0SID == TempRxSID && C1RX0EID == TempRxEID && C1RX0DLC == TempRxDLC && C1R0_flag == 1)) 
		{
			if((C1RX0SID == TempRxSID_G && C1RX0EID == TempRxEID_G && C1RX0DLC == TempRxDLC_G))
				PowerOnFlag  = 1;
			else
				PowerOnFlag = 0;
			C1R0_flag = 0;
			Byte1 = InData0[0] & 0x00FF;
			Byte2 = (InData0[0] & 0xFF00) >> 8;

			Byte3 = InData0[1] & 0x00FF;
			Byte4 = (InData0[1] & 0xFF00) >> 8;

			Byte5 = InData0[2] & 0x00FF;
			Byte6 = (InData0[2] & 0xFF00) >> 8;

			Byte7 = InData0[3] & 0x00FF;
			Byte8 = (InData0[3] & 0xFF00) >> 8;
			FrameSequenceNumber = Byte3;
//			InData0[0] = InData0[1] = InData0[2] = InData0[3] = 0x0000;
			if(Byte1 == 0x10)
			{
				CANFlag = 2;
				NextPacketIndex = 0;
				TotalPackets = Byte4; 
			}
			else if(Byte1 < TotalPackets)
				CANFlag = 2;
			else if(Byte1 >= TotalPackets)
				CANFlag = 3;
			else
			{
				CANFlag = 1;
				NextPacketIndex = 0;
			}
		}	
	}
	else if(CANFlag == 2)
	{
		if(PowerOnFlag == 0)
		{
			TripRecord.Date = Byte2;
			TripRecord.Month = Byte4;		
			TripRecord.Hour = Byte3;
			TripRecord.Minute = Byte6;
			TripRecord.Second = Byte5;
			TripRecord.Year = InData0[3];
		}
		else
			{
	//		LcdDisplay(2," CLEAR TO SEND  ");
	//		Delay1_1s(10);	
			SerialNumber = (unsigned long)Byte2;
			SerialNumber |= (unsigned long)(Byte4 << 8); 
			SerialNumber |= (unsigned long)(Byte3 << 16);
			SerialNumber |= (unsigned long)(Byte6 << 24);
			sprintf(&VehicleID.SerialNo[0],"%08lu",SerialNumber);
			TotalTripCount = InData0[3];
		}
		ClearToSendReceived();
		CANFlag = 1;
	}
	else if(CANFlag == 3)
	{
		CANFlag = 1;
		NextPacketIndex = 0;
		if(PowerOnFlag == 0)
		{
	
			TripRecord.Weight = (unsigned long)Byte2;
			TripRecord.Weight |= (unsigned long)(Byte4 << 8); 
			TripRecord.Weight |= (unsigned long)(Byte3 << 16);
			TripRecord.Weight |= (unsigned long)(Byte6 << 24);
			
			TripRecord.OLStatus = Byte5;
			RecordsUpdated = InData0[3];

			if( (RecordsUpdated == 0) && (Setup.ComMode == 0) )	//CAN
			{
				RecordsUpdated = 0;
				RecordUpdateOver_Flag = 0;
				RecordOverflow_Flag = 0;
				RecordLoopBack_Flag = 0;
				Write_E2(EE_RecordLoopBack,(char*)&RecordLoopBack_Flag,sizeof(RecordLoopBack_Flag));Delay(2);
		
				RecordSent = 0;
				Write_E2(EE_RecordSent,(char*)&RecordSent,2);Delay(2);
				RecordAvailable = 0;
				Write_E2(EE_RecordAvail,(char*)&RecordAvailable,2);Delay(2);
				TotalTripCount = 0;
				Write_E2(EE_TotalTrip,(char*)&TotalTripCount,2);Delay(2);
/*
				memset(&VehicleID.SerialNo[0],' ',36);
				Write_E2(EE_SerialNo,(char*)&VehicleID,36);Delay(2);
				memset(&Optr.Operator[0],' ',28);
				Write_E2(EE_OperatorID,(char*)&Optr,28);Delay(2);

				IDchangeforBroadCastFlag = 1;
				SendCANdata(0xFF,0xFF,0xFFFF,0xFFFF,RecordAvailable);
				IDchangeforBroadCastFlag = 0;
*/
			}

			RecordAvailable = (RecordsUpdated + 1);
			Write_E2(EE_RecordAvail,(char*)&RecordAvailable,sizeof(RecordAvailable));Delay(2);
			
			if(RecordAvailable > (MAXRECORDS))
			{
//				sprintf(PrintBuff," %3d, %3d, %3d  ",RecordAvailable,MAXRECORDS,RecordOverflow_Flag);
//				LcdDisplay(1,PrintBuff);
//				Delay1_1s(25);
				RecordOverflow_Flag = 1;
			}

			if(RecordOverflow_Flag)
				RecordsUpdateLocation = RecordsUpdated % MAXRECORDS;	//Because we receiving the actual records 0 to 9998 (total 9999)
			else
				RecordsUpdateLocation = RecordsUpdated;

//				sprintf(PrintBuff," %3d, %3d, %3d  ",RecordAvailable,MAXRECORDS,RecordsUpdateLocation);
//				LcdDisplay(2,PrintBuff);
//				Delay1_1s(20);

			TripRecord.TempPad = 0;
			TripRecord.TempFlag = 0;
			Write_E2((unsigned long)(EE_DATA_ADD+((unsigned long)(RecordsUpdateLocation * SIZE_REC))),(char*)&TripRecord,SIZE_REC);

			RecordUpdateOver_Flag = 0;
//			RecordAvailable++;
/*	
				sprintf(PrintBuff,"%02u/%02u/%4d",TripRecord.Date,TripRecord.Month,TripRecord.Year);
				Com2StringSend(PrintBuff,10);
				memset(PrintBuff,' ',40);
				Com2StringSend(PrintBuff,5);
				sprintf(PrintBuff,"%02u:%02u",TripRecord.Hour,TripRecord.Minute);
				Com2StringSend(PrintBuff,5);
				memset(PrintBuff,' ',40);
				Com2StringSend(PrintBuff,5);
				sprintf(PrintBuff,"%4d, %06lu",RecordsUpdated,TripRecord.Weight);
				Com2StringSend(PrintBuff,12);
				if(TripRecord.OLStatus)
					memcpy(PrintBuff,"*    ",5);
				else
					memcpy(PrintBuff,"     ",5);
				Com2StringSend(PrintBuff,5);
				memset(PrintBuff,' ',40);
				Com2StringSend(PrintBuff,5);
				sprintf(PrintBuff,"%2d",TripRecord.TempFlag);
				Com2StringSend(PrintBuff,2);
				Com2Bytesend(0x0d);
				Com2Bytesend(0x0a);
*/
		}
		else
		{
//		LcdDisplay(2," CLEAR TO SEND 2");
//			Delay1_1s(10);	
			VehicleID.ModelName[0] = ' ';
			VehicleID.ModelName[1] = ' ';
			VehicleID.ModelName[2] = Byte2;
			VehicleID.ModelName[3] = Byte4;
			VehicleID.ModelName[4] = Byte3;
			VehicleID.ModelName[5] = Byte6;
			VehicleID.ModelName[6] = Byte5;
			VehicleID.ModelName[7] = Byte8;
		}			
		FullDataACK();
//		C1RX0SID=0;C1RX0EID=0;C1RX0DLC = 0;
		if(PowerOnFlag == 1)
		{
//			PowerOnFlag = 0;
			Write_E2(EE_SerialNo,(char*)&VehicleID,36);Delay(2);
			Write_E2(EE_TotalTrip,(char*)&TotalTripCount,2);Delay(2);
//			LcdDisplay(2," CLEAR TO SEND 3");
//			Delay1_1s(10);	
//			CANFlag = 1;
			IDchangeforBroadCastFlag = 1;
//			RecordAvailable = 10;
			SendCANdata(0xFF,0xFF,0xFFFF,0xFFFF,RecordAvailable);
			IDchangeforBroadCastFlag = 0;
		}
//		CANFlag = 0;
		// Please increment the record completion counter
	}
	else
	{
		CANFlag = 0;
	}
}

void Can_Init(void)
{
		CAN1 = 0;
	//--------------------------------------------------------------------------------------------------------------------
						//Initialization of CAN1 Module and Enabling of CAN1 Interrupts
	//--------------------------------------------------------------------------------------------------------------------
	
	 C1CTRLbits.CANCKS = 1;			// Select the CAN Master Clock . It is equal to Fcy here. 
	 								// equal to Fcy.(Fcy=30Mhz)
	
	 C1CFG1bits.SJW=0;				//Synchronized jump width time is 1 x TQ when SJW is equal to 00
		 
	 C1CFG1bits.BRP = 0;		//((FCY/(2*NTQ*BITRATE))-1) 	
	
	 C1CFG2 = 0x04B8;                              // Each bit time is 15Tq
	
	///Interrupt Section of CAN Peripheral
	
	 C1INTF = 0;					//Reset all The CAN Interrupts 
	 IFS1bits.C1IF = 0;  			//Reset the Interrupt Flag status register
	 C1INTE = 0x00FF;               //Enable all CAN interrupt sources
	 IEC1bits.C1IE = 1;				//Enable the CAN1 Interrupt 
	
	
	//-------------------------------------------------------------------------------------------------------------------
						//Initialization of CAN2 Module and Enabling of CAN2 Interrupts
	//-------------------------------------------------------------------------------------------------------------------
	
/*	
	C2CTRLbits.CANCKS = 1;			// Select the CAN Module Frequency Here CAN Master Clk is 
									// equal to Fcy.(Fcy=30Mhz)
	
	C2CFG1bits.SJW=0;			   	//Synchronized jump width time is 1 x TQ when SJW is equal to 00
	 
	C2CFG1bits.BRP = 0;		//((FCY/(2*NTQ*BITRATE))-1)  
	
	C2CFG2 = 0x04B8;                // SEG1PH=6Tq, SEG2PH=3Tq, PRSEG=5Tq
	                                // Sample 3 times
	 // C2CFG2 = 0x04A3;                              // Each bit time is 15Tq
	//CAN2 Interrupt Section
	
	 C2INTF = 0;					//Reset all The CAN Interrupts 
	 IFS2bits.C2IF = 0;  			//Reset the Interrupt Flag status register
	 C2INTE = 0x00FF;               //Enable all interrupt sources
	 IEC2bits.C2IE = 1;				//Enable the CAN2 Interrupt
*/
	//-----------------------------------------------------------------------------------------------------------------------
						// Configure Receive registers, Filters and Masks
	//-----------------------------------------------------------------------------------------------------------------------
	
	 // We are initializing the Receive Buffer 0 and Receive Buffer 1 for CAN1 and CAN2 
	
	 
	 C1RX0CON = C1RX1CON = C2RX0CON = C2RX1CON = 0x0000; 	// Receive Buffer1 and 0 Status
	 														//and Control Register for CAN1 and CAN2
	 	
/*
	C1RXF0SID = C1RXF1SID = C2RXF0SID = C2RXF1SID = 0x0889;		// 0x222 ID only allow
	C1RXF0EIDH = C1RXF1EIDH = C1RXF0EIDL = C1RXF1EIDL = 0x0000;
	C2RXF0EIDH = C2RXF1EIDH = C2RXF0EIDL = C2RXF1EIDL = 0x0000;
	
	C1RXM0SID = C1RXM1SID = C2RXM0SID = C2RXM1SID = 0x0889;		// 0x222 ID only allow
	C1RXM0EIDH = C1RXM1EIDH = C1RXM0EIDL = C1RXM1EIDL = 0x0000;
	C2RXM0EIDH = C2RXM1EIDH = C2RXM0EIDL = C2RXM1EIDL = 0x0000;*/
	 // Acceptance Mask Register0SID and Register1SID associated with Recieve Buffer0
	 // and Receive Buffer1 for CAN1 and CAN2 
	 C1RXM0SID  = C1RXM1SID  = C2RXM0SID  = C2RXM1SID  = 0x0000;	//0x1FFD; //vijay
	 
	 // Acceptance Mask Register0EIDH and Register1EIDH associated with Recieve Buffer0
	 // and Receive Buffer1 for CAN1 and CAN2
	 C1RXM0EIDH = C1RXM1EIDH = C2RXM0EIDH = C2RXM1EIDH = 0x0000;	//0x0FFF; //vijay
	
	 // Acceptance Mask Register0EIDL and Register1EIDL associated with Recieve Buffer0 
	 //and Receive Buffer1 for CAN1 and CAN2
	 C1RXM0EIDL = C1RXM1EIDL = C2RXM0EIDL = C2RXM1EIDL = 0x0000;	//0xFC00; //vijay
	
	 
	//Initializing of Acceptance Filter n Standard Identifier for CAN1
	
/*	 C1RXF0SID 	= 0x0AA8;	//CAN1 Receive Acceptance Filter2 SID 		
	 C1RXF2SID 	= 0x1555;   //CAN1 Receive Acceptance Filter2 SID
	 C1RXF2EIDH = 0x0004;   //CAN1 Receive Acceptace  Filter2 Extended Identifier high byte
	 C1RXF2EIDL = 0x8C00;	//CAN1 Receive Acceptance Filter2 Extended identifier low byte
*/	 
	 C1RXF2SID = 0x0000;
	 C1RXF3SID = 0x0000;
	 C1RXF4SID = 0x0000;

	
	//Initializing of Acceptance Filter n Standard Identifier for CAN2
	
/*	 C2RXF0SID 	= 0x0AA8;	//CAN2 Receive Acceptance Filter0 SID  
	 C2RXF2SID 	= 0x1555;	//CAN2 Receive Acceptance Filter2 SID
	 C2RXF2EIDH = 0x0004; 	//CAN2 Receive Acceptace  Filter2 Extended Identifier high byte
	 C2RXF2EIDL = 0x8C00;	//CAN2 Receive Acceptance Filter2 Extended identifier low byte
*/
/*	 C2RXF2SID = 0x0888;
	 C2RXF3SID = 0x0444;
	 C2RXF4SID = 0x0CCC;
*/		 
	 //-----------------------------------------------------------------------------------------------------------------------
							// Configure Transmit Registers Buffer 0 and Transmit Buffer 1
	//-----------------------------------------------------------------------------------------------------------------------
	  
	
	 C1TX0CON = 0x0003;		// highest interrmediate priority
	 C1TX0SID = 0x4089;     // SID - 0x222  // extended identifier 
	 C1TX0EID = 0x0000;     // EID 
	 C1TX0DLC = 0x01C0;		//Select the Data word Length for CAN1 Transmit Buffer0 which is 8 byte  
	
	 C1TX1CON = 0x0003;		// highest interrmediate priority
	 C1TX1SID = 0x4089;     // SID - 0x222  // extended identifier 
	 C1TX1EID = 0x0000;     // EID 
	 C1TX1DLC = 0x01C0;		//Select the Data word Length for CAN1 Transmit Buffer0 which is 8 byte  

	 C1TX2CON = 0x0003;		// highest interrmediate priority
	 C1TX2SID = 0x4089;     // SID - 0x222  // extended identifier 
	 C1TX2EID = 0x0000;     // EID 
	 C1TX2DLC = 0x01C0;		//Select the Data word Length for CAN1 Transmit Buffer0 which is 8 byte  
	 
	 // Data Field 1,Data Field 2, Data Field 3, Data Field 4 // 8 bytes selected by DLC 
	
	 C1TX0B1 = 0xFFFF;
	 C1TX0B2 = 0xFFFF;
	 C1TX0B3 = 0xFFFF;
	 C1TX0B4 = 0xFFFF;

	 
/*	 C1TX1CON = 0x0002;             // High Intermediate priority
	 C1TX1SID = 0x2044;             // SID - 0x111  
	 C1TX1EID = 0x0000;             // EID                   
	 C1TX1DLC = 0x01C0;				//Select the Data word Length for CAN1 Transmit Buffer1 which
									// is 8 byte
	
	 //Data Field 1, Data Field 2 // 4 bytes selected by DLC
	
	 C1TX1B1 = 0xBBBB; //OutData0[0];
	 C1TX1B2 = 0xBBBB; //OutData0[1];
	 C1TX1B3 = 0xBBBB; //OutData0[2];
	 C1TX1B4 = 0xBBBB; //OutData0[3];


	 C1TX2CON = 0x0001;
	 C1TX2SID = 0x60CC;     // SID 
	 C1TX2EID = 0x0000;     // EID 
	 C1TX2DLC = 0x01C0;		//Select the Data word Length for CAN1 Transmit Buffer0 which is 8 byte  
	 
	 // Data Field 1,Data Field 2, Data Field 3, Data Field 4 // 8 bytes selected by DLC 
	
	 C1TX2B1 = 0xCCCC; //OutData0[0];
	 C1TX2B2 = 0xCCCC; //OutData0[1];
	 C1TX2B3 = 0xCCCC; //OutData0[2];
	 C1TX2B4 = 0xCCCC; //OutData0[3];

*/
//--------------------------------------
/*	 C1TX0CON = 0x0003;             // High Intermediate priority
	 C1TX0SID = 0xA855;             // SID  
	 C1TX0EID = 0x0004;             // EID                   
	 C1TX0DLC = 0x8DA0;				//Select the Data word Length for CAN1 Transmit Buffer1 which
									// is 4 byte
	
	 //Data Field 1, Data Field 2 // 4 bytes selected by DLC
	
	 C1TX0B1 = OutData1[0];
	 C1TX0B2 = OutData1[1];
	
	 C1TX1CON = 0x0002;             // High Intermediate priority
	 C1TX1SID = 0xA855;             // SID  
	 C1TX1EID = 0x0004;             // EID                   
	 C1TX1DLC = 0x8DA0;				//Select the Data word Length for CAN1 Transmit Buffer1 which
									// is 4 byte
	
	 //Data Field 1, Data Field 2 // 4 bytes selected by DLC
	
	 C1TX1B1 = OutData1[0];
	 C1TX1B2 = OutData1[1];

	 C1TX2CON = 0x0001;             // High Intermediate priority
	 C1TX2SID = 0xA855;             // SID  
	 C1TX2EID = 0x0004;             // EID                   
	 C1TX2DLC = 0x8DA0;				//Select the Data word Length for CAN1 Transmit Buffer1 which
									// is 4 byte
	
	 //Data Field 1, Data Field 2 // 4 bytes selected by DLC
	
	 C1TX2B1 = OutData1[0];
	 C1TX2B2 = OutData1[1];
*/
//-----------------------------------------	
	
//---------------------------------------
/*	 C2TX2CON = 0x0002;             // High Intermediate priority
	 C2TX2SID = 0xA855;             // SID = 10101010101 (0x555) 
	 C2TX2EID = 0x0004;             // EID = 1110000000100100011 (0x00123)                  
	 C2TX2DLC = 0x8DA0;             //Select the Data word Length for CAN1 Transmit Buffer0 which is 
									//8 byte    
	
	 //Data Field 1, Data Field 2 // 4 bytes selected by DLC
	 
	 C2TX2B1 = OutData3[0];
	 C2TX2B2 = OutData3[1]; 
*/
//---------------------------------------
	 //Change to Loopback Operation Mode from Configuration Mode
	
/*	 C1CTRLbits.REQOP = 2;
	 C2CTRLbits.REQOP = 2;
	 while(C1CTRLbits.OPMODE != 2);//Wait for CAN1 mode change from Configuration Mode to Loopback mode 
	 while(C2CTRLbits.OPMODE != 2);//Wait for CAN2 mode change from Configuration Mode to Loopback mode
*/
	 C1CTRLbits.REQOP = 0;
//	 C2CTRLbits.REQOP = 0;
	 while(C1CTRLbits.OPMODE != 0);//Wait for CAN1 mode change from Configuration Mode to Normal mode 
//	 while(C2CTRLbits.OPMODE != 0);//Wait for CAN2 mode change from Configuration Mode to Normal mode
	 
	 //Enable transmission
	
//	 C1TX0CONbits.TXREQ = 1;        
//	 C1TX1CONbits.TXREQ = 1;
//	 C1TX2CONbits.TXREQ = 1;
//	 C2TX0CONbits.TXREQ = 1;        
//	 C2TX1CONbits.TXREQ = 1;
//	 C2TX2CONbits.TXREQ = 1;
//	 C1RX0CONbits.RXIE = 1;  
	 C1T0_flag = 1;      
}



//--------------------------------------------------------------------------------------------------------------------------
											//Interrupt Section for CAN1
//--------------------------------------------------------------------------------------------------------------------------

void __attribute__((interrupt, no_auto_psv)) _C1Interrupt(void)
{
      
	  IFS1bits.C1IF = 0;         //Clear interrupt flag

      if(C1INTFbits.TX0IF)
      {
       	 C1T0_flag = 1;
		 C1INTFbits.TX0IF = 0;  //If the Interrupt is due to Transmit0 of CAN1 Clear the Interrupt

      }  
      else if(C1INTFbits.TX1IF)
      {
      	C1T1_flag = 1;  
		C1INTFbits.TX1IF = 0;   //If the Interrupt is due to Transmit1 of CAN1 Clear the Interrupt 

      }  

      if(C1INTFbits.RX0IF)
      {
		C1R0_flag = 1;
//		LcdDispBuff[0] = 'A';      
		C1INTFbits.RX0IF = 0; 	//If the Interrupt is due to Receive0 of CAN1 Clear the Interrupt
		if(C1RX0CONbits.RXRTRRO)
		{
			RTR_flag = 1;			
		}
		else
		{
        	InData0[0] = C1RX0B1; 
        	InData0[1] = C1RX0B2;  	//Move the recieve data from Buffers to InData
        	InData0[2] = C1RX0B3;	
        	InData0[3] = C1RX0B4;
		}
		C1RX0CONbits.RXFUL = 0;
			
      }

      else if(C1INTFbits.RX1IF)
      {      
        
		C1R1_flag = 1;	
//		LcdDispBuff[0] = 'B';	
		C1INTFbits.RX1IF = 0;  	//If the Interrupt is due to Receive1 of CAN1 Clear the Interrupt
		if(C1RX1CONbits.RXRTRRO)
		{
			RTR_flag = 1;			
		}
		else
		{
        	InData1[0] = C1RX1B1;   //Move the  data received to Indata Registers
        	InData1[1] = C1RX1B2;
			InData1[2] = C1RX1B3;
			InData1[3] = C1RX1B4;

		}
		C1RX1CONbits.RXFUL = 0;  // clear indication of buffer free
		        
      }
	  if(C1INTFbits.ERRIF)
	  {
	  	C1INTFbits.ERRIF = 0;		
		C1ER_flag = 1;
	  }
	  if(C1INTFbits.TXWAR)
	  {
		 C1INTFbits.TXWAR = 0;		 
		 C1TXWAR_flag = 1;
	  }
	 if(C1INTFbits.RXWAR)
	  {
		 C1INTFbits.RXWAR = 0;		 
		 C1RXWAR_flag = 1;
	  }
	 if(C1INTFbits.TXEP)
	  {
		 C1INTFbits.TXEP = 0;		 
		 C1TXEP_flag = 1;
	  }
	 if(C1INTFbits.RXEP)
	  {
		 C1INTFbits.RXEP = 0;		 
		 C1RXEP_flag = 1;
	  }
	 if(C1INTFbits.TXBO)
	  {
		 C1INTFbits.TXBO = 0;		 
		 C1TXBO_flag = 1;
	  }
}


//--------------------------------------------------------------------------------------------------------------------------
											//Interrupt Section for CAN2
//--------------------------------------------------------------------------------------------------------------------------

void __attribute__((interrupt, no_auto_psv)) _C2Interrupt(void)
{
      IFS2bits.C2IF = 0;         // Clear interrupt flag

      if(C2INTFbits.TX0IF)
      
	  {
        
		C2T0_flag = 1;
		C2INTFbits.TX0IF = 0;

      }  

      else if(C2INTFbits.TX1IF)

      {
        
		C2T1_flag = 1;
		C2INTFbits.TX1IF = 0;  

      }  

      if(C2INTFbits.RX0IF)
      {      
       
		C2R0_flag = 1;
		C2INTFbits.RX0IF = 0;

        InData2[0] = C2RX0B1;
        InData2[1] = C2RX0B2;
        InData2[2] = C2RX0B3;
        InData2[3] = C2RX0B4;
		C2RX0CONbits.RXFUL = 0;
		
      }

      else if(C2INTFbits.RX1IF)

      {      
        C2R1_flag = 1;
		C2INTFbits.RX1IF = 0;

        InData3[0] = C2RX1B1;
        InData3[1] = C2RX1B2;
		InData3[2] = C2RX1B3;
		InData3[3] = C2RX1B4;
		C2RX1CONbits.RXFUL = 0;
		
               
      }
	 /* else if(C2INTFbits.RX1OVR)
		{
			j++;
		}
	  */
	  if(C2INTFbits.ERRIF)
	  {
	  	C2INTFbits.ERRIF = 0;
		C2ER_flag = 1;
	  }
	  if(C2INTFbits.TXWAR)
	  {
		 C2INTFbits.TXWAR = 0;
		 C2TXWAR_flag = 1;
	  }
	  if(C2INTFbits.RXWAR)
	  {
		 C2INTFbits.RXWAR = 0;
		 C2RXWAR_flag = 1;
	  }
	  if(C2INTFbits.TXEP)
	  {
		 C2INTFbits.TXEP = 0;
		 C2TXEP_flag = 1;
	  }
	  if(C2INTFbits.RXEP)
	  {
		 C2INTFbits.RXEP = 0;
		 C2RXEP_flag = 1;
	  }
	  if(C2INTFbits.TXBO)
	  {
		 C2INTFbits.TXBO = 0;
		 C2TXBO_flag = 1;
	  }
}
void GPRS_Setting_BroadCast_254(void)
{
	CANRecIdentifier_29.Priority = 6;	// constant
	CANRecIdentifier_29.Reserved = 0;	// constant
	CANRecIdentifier_29.DataPage = 0;	// constant
	CANRecIdentifier_29.PF = 236;
	CANRecIdentifier_29.PS = 254;		// For GPRS Remote Settings
	CANRecIdentifier_29.SAddress = Setup.DestID;
	TempRxSID_GS = (CANRecIdentifier_29.Priority << 10) | ((CANRecIdentifier_29.PF >> 2) << 2) | 0x01; 
	TempRxEID_GS = ((CANRecIdentifier_29.PF & 0x03) << 10) | (CANRecIdentifier_29.PS << 2) |((CANRecIdentifier_29.SAddress & 0xC0) >> 6);
	TempRxDLC_GS = ((CANRecIdentifier_29.SAddress & 0x3F) << 10) | (0x0118);
}
void GPRS_Setting(void)
{
	unsigned char PrintBuff[16] = {0};

	if( (C1RX0SID == TempRxSID_GS) && (C1RX0EID == TempRxEID_GS) && (C1RX0DLC == TempRxDLC_GS) && (C1R0_flag == 1) )
	{
//			LcdDisplay(2,"     CAN R      ");Delay1_1s(10);LcdDisplay(2,"                ");
			C1R0_flag = 0;
			Byte1 = InData0[0] & 0x00FF;
			Byte2 = (InData0[0] & 0xFF00) >> 8;

			Byte3 = InData0[1] & 0x00FF;
			Byte4 = (InData0[1] & 0xFF00) >> 8;

			Byte5 = InData0[2] & 0x00FF;
			Byte6 = (InData0[2] & 0xFF00) >> 8;

			Byte7 = InData0[3] & 0x00FF;
			Byte8 = (InData0[3] & 0xFF00) >> 8;

//			InData0[0] = InData0[1] = InData0[2] = InData0[3] = 0x0000;
			if(Byte2 == 0xF0)
			{
				GPRSBroadcast_Flag = 1;
				CTSReceived_GPRSSetting(1);
				GPRSBroadcast_Flag = 0;
			}
			else if(Byte2 == 0xF1)
			{
				GPRS_Setup.MobileOptr = Byte3;
				Write_E2(EE_MobileOptr,(char*)&GPRS_Setup.MobileOptr,1);
				GPRSBroadcast_Flag = 1;
				CAN_ID();
				OutData0[0] = From2Bytes(0x13,0xF0);
				OutData0[1] = From2Bytes(0x0E,0x02);
				OutData0[2] = From2Bytes(0xFF,0x00);
				OutData0[3] = From2Bytes(0xEB,0x00);
				CAN_Send();												//ACK to Tipper
				GPRSBroadcast_Flag = 0;
			}
			else if( (Byte2 == 0xF2) && (Byte5 == 0x00) )
			{
				GPRSBroadcast_Flag = 1;
				CAN_ID();
				Byte5 = (unsigned char)(SignalLevel >> 8) ;
				Byte6 = (unsigned char)((SignalLevel << 8) >> 8); 
				Byte7 = (unsigned char)(GPRS_Flag) ;
				Byte8 = (unsigned char)Modem_Init_State; 
		
				OutData0[0] = From2Bytes(0x11,0xF2);
				OutData0[1] = From2Bytes(0x00,0xFF);
				OutData0[2] = From2Bytes(Byte5,Byte6);
				OutData0[3] = From2Bytes(Byte7,Byte8);
				CAN_Send();	
				GPRSBroadcast_Flag = 0;
			}
			else if(Byte2 == 0xF2)
			{
				GPRSBroadcast_Flag = 1;
				CTSReceived_GPRSSetting(2);
				GPRSBroadcast_Flag = 0;
			}
			else if( (Byte2 == 0xF3) && (Byte3 == 0xFF) )
			{
				ResetValues();
				GPRSBroadcast_Flag = 1;
				CAN_ID();
				OutData0[0] = From2Bytes(0x11,0xF3);
				OutData0[1] = From2Bytes(0xFD,0xFF);
				OutData0[2] = From2Bytes(0xFF,0xFF);
				OutData0[3] = From2Bytes(0xFF,0xFF);
				CAN_Send();
				GPRSBroadcast_Flag = 0;
				LcdDisplay(1," Trips Reseted  ");
				Delay1_1s(10);
			}
			else if(Byte2 == 0xF3)
			{
				GPRSBroadcast_Flag = 1;
				CTSReceived_GPRSSetting(3);
				GPRSBroadcast_Flag = 0;
			}
//			sprintf(PrintBuff,"              %2u",GPRS_Setup.MobileOptr);LcdDisplay(2,PrintBuff);Delay1_1s(10);LcdDisplay(2,"                ");
	}
}


