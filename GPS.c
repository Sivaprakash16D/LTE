#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "GPS.h"


void GPS_Process()
{
    char Store_GPS_Data[200] = {0};
    
//    +QGPSLOC: 105039.0,1258.4595N,08014.9300E,0.7,4.0,2,243.53,0.0,0.0,300617,09
//    sprintf(Store_GPS_Data,"Sec=%04d:%04d",GPS_Getting_Sec,Get_GPS_Timer);
//    LcdDisplay(TOP,Store_GPS_Data);
//    Delay1_1s(1);
    if(GPS_Getting_Sec >= Get_GPS_Timer){
        GPS_Getting_Sec = 0;
        
        
        if(GPS_Flag == 1){
            GPS_Flag = 2;
            memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
            Com1StringSend("AT+QGPS=1",9);
            Com1Bytesend(0x0D);
            Com1Bytesend(0x0A);
            
            
            LcdDisplay(TOP,"   GPS  1       ");
            Delay1_1s(1);
            Com2StringSend(ReceiveBuff1,ReceiveCounter1);
            Com2Bytesend(0x0D);
            Com2Bytesend(0x0A);
    Delay1_1s(1);
        }
        else if(GPS_Flag == 2){
            if(ReceiveBuff1[2]=='O' && ReceiveBuff1[3]=='K'){
                GPS_Flag = 3;
                memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
                LcdDisplay(TOP,"   GPS  2       ");
                Delay1_1s(1);
            }
        }
        else if(GPS_Flag == 3){
            GPS_Flag = 4;
            memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
            Com1StringSend("AT+QGPSLOC?",11);
            Com1Bytesend(0x0D);
            Com1Bytesend(0x0A);
            
            Delay1_1s(1);
            Com2StringSend(ReceiveBuff1,ReceiveCounter1);
            Com2Bytesend(0x0D);
            Com2Bytesend(0x0A);
            Delay1_1s(1);
        }
        else if(GPS_Flag == 4){
            GPS_Getting_Sec = 0;
            if((ReceiveBuff1[3] == 'C')&&(ReceiveBuff1[4] == 'M')&&((ReceiveBuff1[5] == 'E'))&&(ReceiveBuff1[8] == 'R'))
            {
                if((ReceiveBuff1[14] == '5')&&(ReceiveBuff1[15] == '0')&&(ReceiveBuff1[16] == '5')){
                    memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
                    Com2StringSend("ERR505",6);
                    GPS_Flag = 1;
                    GPS_Getting_Sec = 0;
                }
                else if((ReceiveBuff1[14] == '5')&&(ReceiveBuff1[15] == '1')&&(ReceiveBuff1[16] == '6')){
                    memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
                    Com2StringSend("ERR516",6);
                    GPS_Flag = 3;
                    GPS_Getting_Sec = 0;
                }
                else if((ReceiveBuff1[3] == 'Q')&&(ReceiveBuff1[4] == 'G')&&((ReceiveBuff1[5] == 'P'))&&(ReceiveBuff1[6] == 'S'))
                {
                    memset(Store_GPS_Data,' ',200);
                    memcpy(Store_GPS_Data,ReceiveBuff1,ReceiveCounter1);
                    Com2StringSend(ReceiveBuff1,ReceiveCounter1);
                    memset(ReceiveBuff1,' ',350);ReceiveCounter1 = 0;
                    GPS_Flag = 3;
                    GPS_Getting_Sec = 0;
                }
            }
        }
    }
}






