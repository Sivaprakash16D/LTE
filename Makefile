# MPLAB IDE generated this makefile for use with GNU make.
# Project: PayLoadControlUnit.mcp
# Date: Tue Aug 09 18:25:44 2016

AS = pic30-as.exe
CC = pic30-gcc.exe
LD = pic30-ld.exe
AR = pic30-ar.exe
HX = pic30-bin2hex.exe
RM = rm

PayLoadControlUnit.hex : PayLoadControlUnit.cof
	$(HX) "PayLoadControlUnit.cof"

PayLoadControlUnit.cof : LCD_DISP_.o Timer.o PayLoadCU.o E2C_1024.o RTC.o Menu.o Keypad.o GPRS_app.o CAN_App.o CAN_Func.o
	$(CC) -mcpu=30F6012A "LCD_DISP_.o" "Timer.o" "PayLoadCU.o" "E2C_1024.o" "RTC.o" "Menu.o" "Keypad.o" "GPRS_app.o" "CAN_App.o" "CAN_Func.o" -o"PayLoadControlUnit.cof" -Wl,-Tp30F6012A.gld,--defsym=__MPLAB_BUILD=1,-Map="PayLoadControlUnit.map",--report-mem

LCD_DISP_.o : LCD_DISP.h c:/program\ files/microchip/mplab\ c30/include/yvals.h c:/program\ files/microchip/mplab\ c30/include/string.h c:/program\ files/microchip/mplab\ c30/support/dsPIC30F/h/p30f6012A.h LCD_DISP_.c
	$(CC) -mcpu=30F6012A -x c -c "LCD_DISP_.c" -o"LCD_DISP_.o" -g -Wall

Timer.o : Timer.h c:/program\ files/microchip/mplab\ c30/include/stdlib.h c:/program\ files/microchip/mplab\ c30/include/math.h c:/program\ files/microchip/mplab\ c30/include/yvals.h c:/program\ files/microchip/mplab\ c30/include/stdlib.h c:/program\ files/microchip/mplab\ c30/support/generic/h/dsp.h c:/program\ files/microchip/mplab\ c30/support/dsPIC30F/h/p30f6012A.h Timer.c
	$(CC) -mcpu=30F6012A -x c -c "Timer.c" -o"Timer.o" -g -Wall

PayLoadCU.o : Keypad.h E2C_1024.h LCD_DISP.h Includes.h c:/program\ files/microchip/mplab\ c30/include/stdio.h c:/program\ files/microchip/mplab\ c30/include/stdlib.h c:/program\ files/microchip/mplab\ c30/include/string.h c:/program\ files/microchip/mplab\ c30/include/math.h c:/program\ files/microchip/mplab\ c30/include/yvals.h c:/program\ files/microchip/mplab\ c30/include/stdlib.h c:/program\ files/microchip/mplab\ c30/support/generic/h/dsp.h c:/program\ files/microchip/mplab\ c30/support/generic/h/libpic30.h c:/program\ files/microchip/mplab\ c30/support/dsPIC30F/h/p30f6012A.h c:/program\ files/microchip/mplab\ c30/support/dsPIC30F/h/p30fxxxx.h PayLoadCU.c
	$(CC) -mcpu=30F6012A -x c -c "PayLoadCU.c" -o"PayLoadCU.o" -g -Wall

E2C_1024.o : E2C_1024.h c:/program\ files/microchip/mplab\ c30/include/yvals.h c:/program\ files/microchip/mplab\ c30/include/string.h c:/program\ files/microchip/mplab\ c30/support/dsPIC30F/h/p30f6012A.h E2C_1024.c
	$(CC) -mcpu=30F6012A -x c -c "E2C_1024.c" -o"E2C_1024.o" -g -Wall

RTC.o : General_functions.h Disp_LCD.h c:/program\ files/microchip/mplab\ c30/include/stdlib.h c:/program\ files/microchip/mplab\ c30/include/yvals.h c:/program\ files/microchip/mplab\ c30/include/string.h c:/program\ files/microchip/mplab\ c30/support/dsPIC30F/h/p30f6012A.h RTC.C
	$(CC) -mcpu=30F6012A -x c -c "RTC.C" -o"RTC.o" -g -Wall

Menu.o : LCD_DISP.h E2C_1024.h Keypad.h Menu.h c:/program\ files/microchip/mplab\ c30/include/stdio.h c:/program\ files/microchip/mplab\ c30/include/stdlib.h c:/program\ files/microchip/mplab\ c30/include/yvals.h c:/program\ files/microchip/mplab\ c30/include/string.h c:/program\ files/microchip/mplab\ c30/support/dsPIC30F/h/p30f6012A.h Menu.c
	$(CC) -mcpu=30F6012A -x c -c "Menu.c" -o"Menu.o" -g -Wall

Keypad.o : c:/program\ files/microchip/mplab\ c30/include/stdlib.h c:/program\ files/microchip/mplab\ c30/include/yvals.h c:/program\ files/microchip/mplab\ c30/include/string.h c:/program\ files/microchip/mplab\ c30/support/dsPIC30F/h/p30f6012A.h Keypad.c
	$(CC) -mcpu=30F6012A -x c -c "Keypad.c" -o"Keypad.o" -g -Wall

GPRS_app.o : c:/program\ files/microchip/mplab\ c30/include/stdlib.h c:/program\ files/microchip/mplab\ c30/include/string.h c:/program\ files/microchip/mplab\ c30/include/yvals.h c:/program\ files/microchip/mplab\ c30/include/stdio.h GPRS_app.h GPRS_app.c
	$(CC) -mcpu=30F6012A -x c -c "GPRS_app.c" -o"GPRS_app.o" -g -Wall

CAN_App.o : CAN_App.c
	$(CC) -mcpu=30F6012A -x c -c "CAN_App.c" -o"CAN_App.o" -g -Wall

CAN_Func.o : c:/program\ files/microchip/mplab\ c30/support/dsPIC30F/h/p30f6012A.h c:/program\ files/microchip/mplab\ c30/support/dsPIC30F/h/p30fxxxx.h CAN_Func.c
	$(CC) -mcpu=30F6012A -x c -c "CAN_Func.c" -o"CAN_Func.o" -g -Wall

clean : 
	$(RM) "LCD_DISP_.o" "Timer.o" "PayLoadCU.o" "E2C_1024.o" "RTC.o" "Menu.o" "Keypad.o" "GPRS_app.o" "CAN_App.o" "CAN_Func.o" "PayLoadControlUnit.cof" "PayLoadControlUnit.hex"

