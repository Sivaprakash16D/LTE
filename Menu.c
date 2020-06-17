#include <p30f6012A.h>
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "Menu.h"
#include "Keypad.h"
#include "E2C_1024.h"
#include "LCD_DISP.h"

char menutmp[8];
void DoMenu(void)
{		
	long lngval;
	int intval;
	curindex=0;
	memcpy(LcdDispBuff,"     *MENU*                     ",32);
	LcdDisplay(TOP,LcdDispBuff);
	LcdDisplay(BOTTOM,LcdDispBuff+16);
	Delay(5000);
	Bits.Calib_Manl = 0;
	while(1)
	{		
		strncpy(LcdDispBuff,MenuData[curindex].Name,16);		

		LcdDisplay(TOP,LcdDispBuff);
		LcdDisplay(BOTTOM,LcdDispBuff+16);
	
		KeySense();
		if(UpKey)
		{
			KeyRead = 0;
			curindex = (unsigned char)MenuData[curindex].Next;
			if(Bits.Calib_Manl)
			{
				if(curindex == 8)
					curindex = 13;
			}
		}
		else if(MenuKey)
		{
			KeyRead = 0;
			if(curindex == 0)		
				break;
			curindex = (unsigned char)MenuData[curindex].Esc;
			if(Bits.Calib_Manl)
			{
				if(curindex == 4)
				{
					curindex = 12;
					Bits.Calib_Manl = 0;
				}
			}
		}
		else if(EnterKey)
		{
			KeyRead = 0;
			if(curindex == 12)
				Bits.Calib_Manl = 1;
			if(MenuData[curindex].mnuFlags.flags.Numeric)
			{
				switch(MenuData[curindex].mnuType)
				{
					case PtoLong:
					{
						sprintf(menutmp,"%06lu",(MenuData[curindex].Value->Val.ulData));
						memset(menutmp,' ',6-MenuData[curindex].NumDigit);
						
GET_VALUE:				if(GetData(menutmp,MenuData[curindex].NumDigit,MenuData[curindex].mnuFlags.flags.isPassword,MenuData[curindex].mnuFlags.flags.Validation,*MenuData[curindex].Min,*MenuData[curindex].Max,*MenuData[curindex].NumDecimal))
						{
							lngval = atol(menutmp);
						//	memcpy(MenuData[curindex].Value,&lngval,4);
							
							if(curindex == 2)
							{
							//	CurrentCalib.REFmA1 = lngval;
								if(!Getvalues_mA(1,lngval))
									goto GET_VALUE;	
							}
							else if(curindex == 3)
							{
							//	CurrentCalib.REFmA2 = lngval;
								if(!Getvalues_mA(2,lngval))
									goto GET_VALUE;	
							}
			
							memcpy(MenuData[curindex].Value,&lngval,4);
							if(MenuData[curindex].mnuFlags.flags.WriteE2)		
							{
								Write_E2(MenuData[curindex].E2Index,(char*)&lngval,4);	
							}
							curindex = (unsigned char)MenuData[curindex].Enter;
							
						}
						else
							curindex = (unsigned char)MenuData[curindex].Enter;
					
						
						break;
					}
				/*	case PtoLong1:
					{
						sprintf(menutmp,"%07lu",(MenuData[curindex].Value->Val.ulData));
						memset(menutmp,' ',7-MenuData[curindex].NumDigit); 
						
						if(GetData1(menutmp,MenuData[curindex].NumDigit,MenuData[curindex].mnuFlags.flags.isPassword,MenuData[curindex].mnuFlags.flags.Validation,*MenuData[curindex].Min,*MenuData[curindex].Max,*MenuData[curindex].NumDecimal))
						{
							lngval = atol(menutmp);
							memcpy(MenuData[curindex].Value,&lngval,4);
							if(MenuData[curindex].mnuFlags.flags.WriteE2)
							Write_E2(MenuData[curindex].E2Index,(char*)&lngval,4);													
											
							curindex = (unsigned char)MenuData[curindex].Enter;
						}
						else
							curindex = (unsigned char)MenuData[curindex].Enter;
						break;
					}*/
					case PtoInt:
					{
						sprintf(menutmp,"%06u",(MenuData[curindex].Value->Val.uiData));
						memset(menutmp,' ',6-MenuData[curindex].NumDigit);
						if(GetData(menutmp,MenuData[curindex].NumDigit,MenuData[curindex].mnuFlags.flags.isPassword,MenuData[curindex].mnuFlags.flags.Validation,*MenuData[curindex].Min,*MenuData[curindex].Max,*MenuData[curindex].NumDecimal))
						{
							intval = atoi(menutmp);
							memcpy(MenuData[curindex].Value,(char*)&intval,2); 
							if(MenuData[curindex].mnuFlags.flags.WriteE2)
								Write_E2(MenuData[curindex].E2Index,(char*)&intval,2);
							if(curindex == 13)
							{	
						//		Calibration.Zero1 = intval;
						//		Write_E2(EE_Zero1,(char*)&Calibration.Zero1,2);	
							}
							curindex = (unsigned char)MenuData[curindex].Enter;
						}
						else
							curindex = (unsigned char)MenuData[curindex].Enter;
						break;
					}
					case Combo:
					{
						menutmp[0] = GetCombo(curindex);
						if(menutmp[0])
						{
							MenuData[curindex].Value->Val.ucData = menutmp[0];
			
							if(MenuData[curindex].mnuFlags.flags.WriteE2)
								Write_E2(MenuData[curindex].E2Index,(char*)&menutmp,1);
							curindex = (unsigned char)MenuData[curindex].Enter;
						}
						else
							curindex = (unsigned char)MenuData[curindex].Enter;					

						break;
					}
				/*	case ComboStr:
					{
						menutmp[0] = GetComboString(curindex);
						if(menutmp[0])
						{
							MenuData[curindex].Value->Val.ucData = menutmp[0];
		//					MenuData[curindex].Value = menutmp[0];
							if(MenuData[curindex].mnuFlags.flags.WriteE2)
								Write_E2(MenuData[curindex].E2Index,menutmp,1);								
								
							if(curindex == 80)	//for update the ADC sample per sec, reset the adc
							{
								GainVal=cmbdata[Setup.AdcGain-1];
								AdcSample=cmbdatatostr[Setup.AdcSamplePerSec-1];
								
							//	ConfigureADCGain(GainVal);
								ConfigureADCGain(GainVal,AdcSample);		
								
								ResetAdc();	
								for(i=0;i<7;i++)
									Calib.InsAdc=GetAdc();	
									
								LcdDisplay(TOP,"    ADC SPEED   ");
							}	
							curindex = (unsigned char)MenuData[curindex].Enter;
						}
						else
							curindex = (unsigned char)MenuData[curindex].Enter;
							
						
						break;
					}*/
					case SplFunction:
					{
						switch((int)(MenuData[curindex].Value->Val.uiData))
						{							
							case 0:
							{
								ShowAD(1);
								break;
							}
							case 1:
							{
								ShowAD(2);
								break;
							}
							case 2:
							{
								ShowAD(3);
								break;
							}
							case 3:
							{
						 	//	CheckMem();
								break;
							}
							case 4:
							{
								PortCheck(1);
								break;
							}
							case 5:
							{
								PortCheck(2);
						 		break;
							}
							case 6:
							{
								KeypadTest();
								memset(LcdDispBuff+16,' ',16);
						 		break;
							}
							case 7:
							{
								DIPCheck();
						 		break;
							}
							case 8:
							{
								ChannelConfiguration();
						 		break;
							}
							case 9:							// Set Date
							{
								Set_Date();
						 		break;
							}
							case 10:						// Set Time
							{
								Set_Time();
						 		break;
							}
							case 11:						// RTC Initialize
							{
								RTC_Init();
						 		break;
							}
							case 12:						// LOAD DEFAULT
							{
								LoadDefault();
						 		break;
							}
							case 13:
							{
								DOPCheck();
								break;
							}
							case 14:
							{
								DisplayCheck();
								break;
							}
							case 15:
							{
								RTC_Check();
								break;
							}
							case 16:
							{
								ILSelect();
								break;
							}
							case 17:
							{
								DumpDisplaySel();
								break;
							}
							case 18:
							{
								SelMobileOperator();
								break;
							}
							case 19:
							{
						 	//	DumperRunTime();
								ClearRunTime();
								break;
							}  
							case 20:
							{
								SetComMode();
								break;
							}	
							case 21:
							{
								SetDeviceID();
								break;
							}
							case 22:
							{
								SetDestinationID();
								break;
							}
						}
						curindex = (unsigned char)MenuData[curindex].Enter;
					}
					
				}
				
 			}
			else
				curindex = (unsigned char)MenuData[curindex].Enter;			
			
			if(Bits.Calib_Manl)
			{
				if(curindex == 8)
					curindex = 13;
			}
			
		}
		ReadRTC_DateTime();	//To update the Run time
//		RunTimeUpdate();

		if(TimeOutFlag.TimeOut)
				return;

	}		
	Bits.Calib_Manl = 0;

}

char GetCombo(char x)
{
	char digit;
	unsigned int blink = 0;

	digit = (char)(MenuData[x].Value->Val.ucData-1);
//	digit = (char)(MenuData[x].Value->Val.ucData);
	while(1)
	{

		blink++;
		if(blink<=200) //90
		{
			sprintf(LcdDispBuff+16,"          %6u",cmbdata[digit]);
		}	
		else //if(blink>400) //90
		{
			memset(LcdDispBuff+16,' ',16);
		}

		LcdDisplay(BOTTOM,LcdDispBuff+16);	

		if(blink>400) //160
			blink=0;
	
		KeySense();
		if(EnterKey)
		{
			memset(LcdDispBuff+16,' ',16);
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			TimeOutFlag.TimeOutEnb = 1;TimeOutFlag.TimeOut = 0;TimeOut_Cnt = 0;
			return((unsigned char)(digit+1));
		}
		else if(MenuKey)
		{
			memset(LcdDispBuff+16,' ',16);
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			TimeOutFlag.TimeOutEnb = 1;TimeOutFlag.TimeOut = 0;TimeOut_Cnt = 0;
			return(0);
		}
		else if(UpKey)
		{
			if(digit == *(MenuData[x].Max))
				digit = (char)(*(MenuData[x].Min));
			else
				digit++;			
		}
		TimeOutFlag.TimeOutEnb = 0;TimeOutFlag.TimeOut = 0;TimeOut_Cnt = 0;	// To Check
		if(TimeOutFlag.TimeOut)
			return(0);
	}
	return(0);
}
char GetData(char* buff,char x,char mode,char flag,long minval,long maxval,char dept)
{
	char scanbuff[10];
	unsigned int blink = 0;
	int digit;
	int scanval;
	unsigned char TempBuff[10];
//	unsigned char KeyPressed = 0; 

	digit=5;
	buff[6]='\0';

	if(mode == 0)
	{
		for(scanval=0;scanval<6;scanval++)
			scanbuff[scanval]=buff[scanval];
	}
	else
	{
		for(scanval=0;scanval<6;scanval++)
		{
			scanbuff[scanval] = '0';
			buff[scanval] = '0';
		}
	}

	while(1)
	{
		blink++;

		memcpy(LcdDispBuff+16,"                ",16);
		
		if(PasswordEntry == 1)
		{
			strncpy(LcdDispBuff+29,scanbuff+3,3);
		}
		else
			strncpy(LcdDispBuff+26,scanbuff,6);
		
		if(blink>200) //90
			(LcdDispBuff+26)[digit] = '_';

		if(dept != 0)
		{
			memcpy(TempBuff,LcdDispBuff+26,6);
	//		PutDp((unsigned char*)&LcdDispBuff+26,(long)dept,x,1);	
			PutDp(TempBuff,(long)dept,6,1);	
			memcpy(LcdDispBuff+25,TempBuff,7);	
			
		}
		LcdDisplay(BOTTOM,LcdDispBuff+16);
		
		if(blink>=400)//160
			blink=0;

		KeySense();
		if(EnterKey)
		{   
//			KeyPressed = 1;
			if(mode==1)
				return(1);
			else
			{	
				if((flag==1) & ((atol(buff) < minval) | (atol(buff) > maxval)))
				{					
				
					memcpy(LcdDispBuff+16,"     VALUE ERROR",16);
					LcdDisplay(BOTTOM,LcdDispBuff+16);
					Delay(200000);
			
				}
				else
				{
					memset(LcdDispBuff+16,' ',16);
					LcdDisplay(BOTTOM,LcdDispBuff+16);
					TimeOutFlag.TimeOutEnb = 1;TimeOutFlag.TimeOut = 0;TimeOut_Cnt = 0;
					return(1);
				}
			}
		}
		else if(MenuKey)
		{
//			KeyPressed = 1;
			memset(LcdDispBuff+16,' ',16);
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			TimeOutFlag.TimeOutEnb = 1;TimeOutFlag.TimeOut = 0;TimeOut_Cnt = 0;
			return(0);
		}
		else if(UpKey)
		{
//			KeyPressed = 1;
			if((curindex != 12)) // 12,22 --> decimalpoint // && (curindex != 22)
			{			
				if((digit != 0) || (mode == 1))
				{
					if(buff[digit]=='9')
						buff[digit]='0';
					else
						buff[digit]+= 1;
				}
				else 
				{
					if(buff[digit]=='1')
						buff[digit]='0';
					else
						buff[digit]+= 1;
				}
				scanbuff[digit]=buff[digit];
				blink=0;			
			}
			else 
			{
				if(buff[digit]=='4')
					buff[digit]='0';
				else
					buff[digit]+= 1;
				scanbuff[digit]=buff[digit];
				blink=0;
			}
		}
		else if(LeftKey)
		{
//			KeyPressed = 1;
			if(mode==1)
				scanbuff[digit]='-';
			if((6-x)<digit)
				digit--;
			else
				digit=5;
			blink=0;
		}
		if(curindex >= 5 && curindex <= 14)
		{
			TimeOutFlag.TimeOutEnb = 0;TimeOutFlag.TimeOut = 0;TimeOut_Cnt = 0;			
		}	
		if(TimeOutFlag.TimeOut)
		{
			return(0);
		}

	}
	TimeOutFlag.TimeOutEnb = 1;TimeOutFlag.TimeOut = 0;TimeOut_Cnt = 0;
	return(0);
}

void ShowAD(char val)
{
/*	int TempADC = 0;

//	Select.Channel1 = Select.Channel2 = Select.Channel3 = Select.Channel4 = 1;
	if(val == 3)
	{
		if(Setup.Channel1 == 0 && Setup.Channel2 == 0 && Setup.Channel3 == 0 && Setup.Channel4 == 0)
		{
			LcdDisplay(TOP,"  All Channels  ");
			LcdDisplay(BOTTOM,"    Disabled    ");
			Delay_ms(100);Delay_ms(100);Delay_ms(100);Delay_ms(100);
			return;
		}
		memset(LcdDispBuff,' ',32);
	}
	while(1)
	{
		if(val == 3)
		{
			if(Setup.Channel1 == 1)
			{
			//	Delay_ms(15);
				ReadADCCounts(8);
				TempADC = (unsigned int)ADCSampleVal;
				memcpy(LcdDispBuff,"C1:",3);
				if(TempADC >= MAXADC)
					memcpy(LcdDispBuff+3,"OR  ",4);
				else if(TempADC <= 10)
					memcpy(LcdDispBuff+3,"UR  ",4);
				else
					sprintf(LcdDispBuff+3,"%4u",TempADC);
			}
			if(Setup.Channel2 == 1)
			{
			//	Delay_ms(15);
				ReadADCCounts(9);
				TempADC = (unsigned int)ADCSampleVal;
				LcdDispBuff[7] = ' ';
				memcpy(LcdDispBuff+8,"C2:",3);
				if(TempADC >= MAXADC)
					memcpy(LcdDispBuff+11,"OR  ",4);
				else if(TempADC <= 10)
					memcpy(LcdDispBuff+11,"UR  ",4);
				else
					sprintf(LcdDispBuff+11,"%4u",TempADC);
			}
			if(Setup.Channel3 == 1)
			{
			//	Delay_ms(15);
				ReadADCCounts(10);
				TempADC = (unsigned int)ADCSampleVal;
				LcdDispBuff[15] = ' ';
				memcpy(LcdDispBuff+16,"C3:",3);
				if(TempADC >= MAXADC)
					memcpy(LcdDispBuff+19,"OR  ",4);
				else if(TempADC <= 10)
					memcpy(LcdDispBuff+19,"UR  ",4);
				else
					sprintf(LcdDispBuff+19,"%4u",TempADC);
			}
			if(Setup.Channel4 == 1)
			{
			//	Delay_ms(15);
				ReadADCCounts(11);
				TempADC = (unsigned int)ADCSampleVal;
				LcdDispBuff[23] = ' ';
				memcpy(LcdDispBuff+24,"C4:",3);
				if(TempADC >= MAXADC)
					memcpy(LcdDispBuff+27,"OR  ",4);
				else if(TempADC <= 10)
					memcpy(LcdDispBuff+27,"UR  ",4);
				else
					sprintf(LcdDispBuff+27,"%4u",TempADC);
			}
			LcdDisplay(TOP,LcdDispBuff);
			LcdDisplay(BOTTOM,LcdDispBuff+16);		
		}
		else
		{
			CalcADC();
			sprintf(LcdDispBuff+16,"          %6u",Calibration.RAWADC);
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			TimeOutFlag.TimeOutEnb = 0;TimeOutFlag.TimeOut = 0;TimeOut_Cnt = 0;	
		}

		KeySense();
		if(MenuKey)
		{
			memset(LcdDispBuff+16,' ',16);
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			break;
        }
		else if(EnterKey)
		{
			if(val == 1)
			{			
				Calibration.Zero  = Calibration.RAWADC;
				Calibration.Zero1 = Calibration.RAWADC;
				memset(LcdDispBuff+16,' ',16);
				LcdDisplay(BOTTOM,LcdDispBuff+16);
				Write_E2(EE_Zero,(char*)&Calibration.Zero,sizeof(Calibration.Zero));
				Write_E2(EE_Zero1,(char*)&Calibration.Zero1,sizeof(Calibration.Zero1));	
				break;
			}
			else if(val == 2)
			{
				if(Calibration.RAWADC == Calibration.Zero)
				{
					memcpy(LcdDispBuff+16,"        NO LOAD ",16);
					LcdDisplay(BOTTOM,LcdDispBuff+16);
					Delay(80000);Delay(80000);Delay(80000);Delay(80000);Delay(80000);Delay(80000);
					Delay(80000);Delay(80000);Delay(80000);Delay(80000);Delay(80000);Delay(80000);
					memset(LcdDispBuff+16,' ',16);
					LcdDisplay(BOTTOM,LcdDispBuff+16);
				}
				else
				{
					Calibration.Span  = Calibration.RAWADC;
					memset(LcdDispBuff+16,' ',16);
					LcdDisplay(BOTTOM,LcdDispBuff+16);
					Write_E2(EE_Span,(char*)&Calibration.Span,sizeof(Calibration.Span));
					break;
				}
			}			
		}
		else if(UpKey)
		{
			if(val == 3)
			{
				if(Show_mA())
					break;
			}
		}
		if(TimeOutFlag.TimeOut)
			return;
	}
	TimeOutFlag.TimeOutEnb = 1;TimeOutFlag.TimeOut = 0;TimeOut_Cnt = 0;
*/
	return;
}

void KeypadTest()
{
	while(1)
	{
		KeySense();
		if(MenuKey)
		{
			memcpy(LcdDispBuff+16,"    MENU/ESC    ",16);
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			Delay1_1s(1);
			memcpy(LcdDispBuff+16,"                ",16);
			return;
		}
		else if(EnterKey)
		{
			memcpy(LcdDispBuff+16,"   ENTER/TARE   ",16);
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			Delay1_1s(1);
			memcpy(LcdDispBuff+16,"                ",16);
		}
		else if(UpKey)
		{
			memcpy(LcdDispBuff+16,"       UP       ",16);
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			Delay1_1s(1);
			memcpy(LcdDispBuff+16,"                ",16);
		}
		else if(LeftKey)
		{
			memcpy(LcdDispBuff+16,"      LEFT      ",16);
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			Delay1_1s(1);
			memcpy(LcdDispBuff+16,"                ",16);
		}
		LcdDisplay(BOTTOM,LcdDispBuff+16);
		if(TimeOutFlag.TimeOut)
			return;
	}
	return;
}

void ChannelConfigSel()
{
	unsigned int DlyVal = 60000;
	
	while(DlyVal-- > 0)
	{
		KeySense();
		if(MenuKey)
		{
			LcdDisplay(TOP," ENTER PASSWORD ");
			LcdDisplay(BOTTOM,"  (CH. Config)  ");
			Delay1_1s(2);
			if(GetPassword())
			{
				ChannelConfiguration();
				break;
			}
			else
				break;	
		}
	}
	return;

}

void ChannelConfiguration()
{
	unsigned char CtNo = 1;
	ClearLcdDisplay(TOP_BOTTOM);
	memset(LcdDispBuff,' ',32);
	while(1)
	{
		KeySense();
		if(MenuKey)
		{
			break;
		}
		else if(UpKey)
		{
			if(CtNo++ >= 4)
				CtNo = 1;
		}
		else if(EnterKey)
		{
			ChannelSelect(CtNo);
			memset(LcdDispBuff+16,' ',16);
		}
		
		if(CtNo == 1)
			memcpy(LcdDispBuff,"*   CHANNEL 1  *",16);	
		else if(CtNo == 2)
			memcpy(LcdDispBuff,"*   CHANNEL 2  *",16);	
		else if(CtNo == 3)
			memcpy(LcdDispBuff,"*   CHANNEL 3  *",16);	
		else if(CtNo == 4)
			memcpy(LcdDispBuff,"*   CHANNEL 4  *",16);	

		LcdDisplay(TOP,LcdDispBuff);
		if(TimeOutFlag.TimeOut)
			return;
	}
	
}





void DIPCheck()
{
	memcpy(LcdDispBuff+16," I1-     I2-    ",16); 
	while(1)
	{
		KeySense();
		if(MenuKey)
		{
			break;
		}
		if(_RC2 != 1)	//Input 1
			memcpy(LcdDispBuff+20,"ON ",3);
		else
			memcpy(LcdDispBuff+20,"OFF",3);
		
		if(_RC1 != 1)	//Input 2
			memcpy(LcdDispBuff+28,"ON ",3);
		else
			memcpy(LcdDispBuff+28,"OFF",3);

		LcdDisplay(BOTTOM,LcdDispBuff+16);

		if(TimeOutFlag.TimeOut)
			return;
	}
	memset(LcdDispBuff+16,' ',16);
	return;
}

void DOPCheck()						//OUTPUT1 - RB5, OUTPUT2 - RB4	//LED - RG6,RG7,RG8
{
	unsigned char DisplayFlag = 0;
	
	_RB5 = 0;Nop();
	_RB4 = 0;Nop();
	_RG6 = 1;Nop();
	_RG7 = 1;Nop();
	_RG8 = 1;Nop();
	while(1)
	{
		if(DisplayFlag <= 4)
			memcpy(LcdDispBuff+16,"OP1-     OP2-   ",16); 
		else
			memcpy(LcdDispBuff+16," L1-  L2-  L3-  ",16);  
		KeySense();
		if(MenuKey)
		{
			_RB5 = 0;Nop();
			_RB4 = 0;Nop();
			_RG6 = 0;Nop();
			_RG7 = 0;Nop();
			_RG8 = 0;Nop();
			memset(LcdDispBuff+16,' ',16);
			break;
		}
		else if(UpKey)
		{
			DisplayFlag++;		
			if(DisplayFlag >= 11)
			{
				DisplayFlag = 0;
			}
		}		

		if(DisplayFlag == 1)
		{		
			_RB5 = 1;Nop();Nop();
			memcpy(LcdDispBuff+20,"ON ",3);	
			memcpy(LcdDispBuff+29,"OFF",3);			
		}
		else if(DisplayFlag == 3)
		{
			_RB4 = 1;Nop();Nop();
			memcpy(LcdDispBuff+20,"OFF",3);
			memcpy(LcdDispBuff+29,"ON ",3);			
		}
		else if(DisplayFlag == 6)
		{
//			memcpy(LcdDispBuff+16,"    LED CHECK   ",16); 
			LcdDispBuff[20] = '1';
			LcdDispBuff[25] = '0';
			LcdDispBuff[30] = '0';
			_RG6 = 1;Nop();	//Green

		}
		else if(DisplayFlag == 8)
		{
			LcdDispBuff[20] = '0';
			LcdDispBuff[25] = '1';
			LcdDispBuff[30] = '0';
			_RG7 = 1;Nop();		//Amber


		}
		else if(DisplayFlag == 10)
		{
			LcdDispBuff[20] = '0';
			LcdDispBuff[25] = '0';
			LcdDispBuff[30] = '1';
			_RG8 = 1;Nop();		//Red

		} 
		else
		{
			_RB4 = 0;Nop();
			_RB5 = 0;Nop();
			_RG6 = 0;Nop();
			_RG7 = 0;Nop();
			_RG8 = 0;Nop();
			if(DisplayFlag <= 4)
			{
				memcpy(LcdDispBuff+20,"OFF",3);
				memcpy(LcdDispBuff+29,"OFF",3);	
			}
			else
			{
				LcdDispBuff[20] = '0';
				LcdDispBuff[25] = '0';
				LcdDispBuff[30] = '0';
			}
		}
		LcdDisplay(BOTTOM,LcdDispBuff+16);
		if(TimeOutFlag.TimeOut)
			return;
	}

}

void DisplayCheck()
{
	unsigned char DispCnt = 0;
	unsigned long DispCh = 0;	
	while(1)
	{
		KeySense();
		if(MenuKey)
		{
			memset(LcdDispBuff,' ',32);
			break;
		}
		else if(EnterKey)
		{
			if(DispCnt >= 4)
				DispCnt = 0;
		}
		if(DispCnt < 4)
		{		
			if(DispCh == 0)
			{
				memset(LcdDispBuff,0xFF,32);
				LcdDisplay(TOP,LcdDispBuff);
				LcdDisplay(BOTTOM,LcdDispBuff+16);
			}
			else if(DispCh == 120000)
			{
				memset(LcdDispBuff,' ',32);
				LcdDisplay(TOP,LcdDispBuff);
				LcdDisplay(BOTTOM,LcdDispBuff+16);
			}
			if(DispCh++ >= 160000)
			{
				DispCh = 0;
				DispCnt++;
			}
		}
		else
		{
			LcdDisplay(TOP," Do you want to ");
			LcdDisplay(BOTTOM,"  flash again?  ");
		}	
		if(TimeOutFlag.TimeOut)
			return;
	}

}

void RTC_Check()
{
	char display = 0;
	while(1)
	{
		KeySense();
		if(MenuKey)
		{
			memset(LcdDispBuff+16,' ',32);
			break;
		}
		else if(UpKey)
		{
			if(display++ > 0)
				display = 0;
		}
		ReadRTC_DateTime();
		if(display == 0)
			sprintf(LcdDispBuff+16," Date:%02u/%02u/%04u",RTC.Date,RTC.Month,RTC.Year);
		else
			sprintf(LcdDispBuff+16," Time: %02u:%02u:%02u ",RTC.Hour,RTC.Minute,RTC.Second);

		LcdDisplay(BOTTOM,LcdDispBuff+16);
		if(TimeOutFlag.TimeOut)
			return;
	}	
}



void RTC_Init()
{
	char TempYear[2]={'1','2'};
	while(1)
	{
		memcpy(LcdDispBuff+16,"  Are You Sure? ",16);

		LcdDisplay(BOTTOM,LcdDispBuff+16);
		
		KeySense();
		if(MenuKey)
		{
			break;
		}
		else if(EnterKey)
		{
			memcpy(LcdDispBuff+16,"  Initializing  ",16);
			LcdDisplay(BOTTOM,LcdDispBuff+16);

			RTC.Date=1;
			RTC.Month=8;
			//RTC.Year=2012;
			RTC.Year = AsciitoBcd(TempYear);
			WriteToRTC_Date(); Delay(1000);
		
			RTC.Hour=1;
			RTC.Hour |= 0x80;			// setting as 24 hr format
			RTC.Minute=0;
			RTC.Second=0;
			WriteToRTC_Time(); Delay(1000);
			break;
		}
		if(TimeOutFlag.TimeOut)
			return;
	}
	memset(LcdDispBuff+16,' ',16);
	return;
}

void Set_Date()
{
	char LocalBuff1[7];
	char LocalBuff2[7];
	char LocalBuff3[7];
	char LocalBuff4[7];	

	ReadRTC_DateTime();
	memcpy(LcdDispBuff+16,"Date:           ",16);
	
	sprintf(LocalBuff1,"%02u%02u%02u",RTC.Date,RTC.Month,(RTC.Year % 100));
	memcpy(LocalBuff2,DateFormatMin,6);
	memcpy(LocalBuff3,DateFormatMax,6);
	memcpy(LocalBuff4,DateFormatComp,6);
	if(GetRTCData(2,LocalBuff1,LocalBuff2,LocalBuff3,LocalBuff4))
	{
		WriteToRTC_Date();
	}
	memcpy(LcdDispBuff+16,"                ",16);
	return;
}

void Set_Time()
{
	char LocalBuff1[7];
	char LocalBuff2[7];
	char LocalBuff3[7];
	char LocalBuff4[7];


	ReadRTC_DateTime();
	memcpy(LcdDispBuff+16,"Time:           ",16);
	sprintf(LocalBuff1,"%02u%02u%02u",RTC.Hour,RTC.Minute,RTC.Second);	
	memcpy(LocalBuff2,TimeFormat24hrMin,6);
	memcpy(LocalBuff3,TimeFormat24hrMax,6);
	memcpy(LocalBuff4,TimeFormat24hrComp,6);

	if(GetRTCData(1,LocalBuff1,LocalBuff2,LocalBuff3,LocalBuff4))
	{
		WriteToRTC_Time();
	}
	memcpy(LcdDispBuff+16,"                ",16);
	return;

}


char GetRTCData(char x,char* buff,char* MinBuff,char* MaxBuff,char* CompBuff)
{
	unsigned char scanbuff[10];
//	unsigned char blink=0;
	unsigned int blink=0;

	unsigned char digit=5;
//	unsigned char Validate[6]={0};

	buff[6]='\0';
	if(x==1)
		LcdDispBuff[26]=LcdDispBuff[29]=':';//for time
	else if((x == 2) || (x == 3))
		LcdDispBuff[26]=LcdDispBuff[29]='/';//for date
	LcdDisplay(TOP,LcdDispBuff);
	while(1)
	{
		blink++;
		memcpy(scanbuff,buff,6);

		if(blink>200) 
			scanbuff[digit] = '_';

		if(blink>=400)
			blink=0;		
	
		memcpy(LcdDispBuff+24,scanbuff,2);
		memcpy(LcdDispBuff+27,scanbuff+2,2);
		memcpy(LcdDispBuff+30,scanbuff+4,2);
		
		LcdDisplay(BOTTOM,LcdDispBuff+16);

		KeySense();
		if(MenuKey)
		{
			return(0);
		}
		else if(EnterKey)
		{
			if(x==1)
			{
				RTC.Hour=AsciitoBcd(buff);
				RTC.Hour |= 0x80;			// setting as 24 hr format
				RTC.Minute=AsciitoBcd(buff+2);
				RTC.Second=AsciitoBcd(buff+4);
				return(1);
			}
			else if(x==2)
			{
				RTC.Date=AsciitoBcd(buff);
				RTC.Month=AsciitoBcd(buff+2);
				RTC.Year=AsciitoBcd(buff+4);
				return(1);
			}
			else if(x==3)
			{
			//	memcpy(Validate,buff+4,2);
			//	memcpy(Validate+2,buff+2,2);
			//	memcpy(Validate+4,buff,2);
				if(atol(buff) > 0)
					return(1);
			}
			
		}
		else if(UpKey)
		{
			if(CompBuff[digit]!='N' && buff[digit-1]<CompBuff[digit])
			{
				if(buff[digit]=='9')
				{
					if(buff[digit-1]=='0')
						buff[digit]=MinBuff[digit];
					else
						buff[digit]='0';
				}
				else
					buff[digit]+=1;
			}
			else
			{
				if(buff[digit]>=MaxBuff[digit])
					buff[digit]='0';
				else
					buff[digit]+= 1;
			}
			if(CompBuff[digit]=='N')
			{
			 	if(buff[digit+1] >MaxBuff[digit+1] && buff[digit]>=MaxBuff[digit])
				buff[digit+1]=MaxBuff[digit+1];

				if(buff[digit+1] <MinBuff[digit+1] && buff[digit]=='0')
				buff[digit+1]=MinBuff[digit+1];
			}
			blink=0;
		}
		else if(LeftKey)
		{
			if(digit==0)
				digit=5;
			else 
				digit--;

			blink=0;
		}
		if(TimeOutFlag.TimeOut)
			return(0);
	}
	return(0);
}



char GetPassword()
{
	char EntryPassword[7] = {0};

	memcpy(LcdDispBuff," ENTER PASSWORD ",16); 
	LcdDisplay(TOP,LcdDispBuff);
	PasswordEntry = 1;

	if(GetData(EntryPassword,3,1,0,0,0,0))
	{
		PasswordEntry = 0;
		EntryPassword[6]='\0';
		
		if(RunTimeClr == 1 && atol(EntryPassword) == 555)
			return(1);
		else if((atol(EntryPassword)== 795 || atol(EntryPassword) == Setup.Password) && RunTimeClr == 0)
			return(1);
		else
		{
			memcpy(LcdDispBuff+16,"      ERROR     ",16);  
			LcdDisplay(BOTTOM,LcdDispBuff+16);
			Delay1_1s(1);
			return(0);
		}
		if(TimeOutFlag.TimeOut)
			return(0);
	}

	return(0);
}


