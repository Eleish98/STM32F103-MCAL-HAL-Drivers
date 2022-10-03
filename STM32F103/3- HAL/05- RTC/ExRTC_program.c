/*
 * External_RTC.c
 *
 *  Created on: Apr 19, 2022
 *      Author: 20109
 */

#include "1- LIB/STD_TYPES.h"
#include "1- LIB/BIT_MATH.h"
#include "1- LIB/String_interface.h"

#include "2- MCAL/ST/04- RCC/RCC_interface.h"
#include "2- MCAL/ST/19- I2C/I2C_interface.h"

#include "ExRTC_interface.h"
#include "ExRTC_private.h"



void HRTC_voidInit(ERTC_Config_t* RTC){

	ERTCCalender_t ClockCalender, CompileCalender;
	ERTCTime_t ClockTime, CompileTime;

	MI2C_voidInit(RTC->I2C,I2C_MODE_I2C,MRCC_u32GetBusClock(RCC_BUS_APB1)/1000000,50000);

	HRTC_voidGetDateTime(RTC,&ClockCalender, &ClockTime);

	HRTC_voidStringtoDateTime(__DATE__,__TIME__,&CompileCalender,&CompileTime);

	if(PERTC_s8CompareTime(&CompileCalender,&CompileTime,&ClockCalender,&ClockTime) == 1)
		HRTC_voidSetDateTime(RTC,&CompileCalender,&CompileTime);
}


void HRTC_voidGetDateTime(ERTC_Config_t* RTC, ERTCCalender_t* Calender, ERTCTime_t* Time){

	I2C_Status_t Status = I2C_STATUS_OK;
	u8 Local_u8Buffer[8];
	Local_u8Buffer[0] = 0;
	Status = MI2C_enuConnect(RTC->I2C,EXTERNAL_RTC_I2C_ADDRESS,I2C_DIRECTION_WRITE);
	Status = MI2C_enuWriteNoStopSynch(RTC->I2C,Local_u8Buffer,1);
	Status = MI2C_enuConnect(RTC->I2C,EXTERNAL_RTC_I2C_ADDRESS,I2C_DIRECTION_READ);
	Status = MI2C_enuReadNoStopSynch(RTC->I2C,Local_u8Buffer,7);
	Status = MI2C_enuDisconnect(RTC->I2C);


	PERTC_voidRegistersToDateTime(Local_u8Buffer, Calender, Time);
	(void)Status;
}

void HRTC_voidSetDateTime(ERTC_Config_t* RTC, ERTCCalender_t* Calender, ERTCTime_t* Time){

	u8 Local_u8Buffer[8];
	I2C_Status_t Status = I2C_STATUS_OK;
	//Write time to Clock
	Local_u8Buffer[0] = 0;
	PERTC_voidDateTimeToRegisters(Local_u8Buffer+1,Calender,Time);
	Status = MI2C_enuConnect(RTC->I2C,EXTERNAL_RTC_I2C_ADDRESS,I2C_DIRECTION_WRITE);
	Status = MI2C_enuWriteNoStopSynch(RTC->I2C,Local_u8Buffer,8);
	Status = MI2C_enuDisconnect(RTC->I2C);
	(void)Status;
}

void HRTC_voidStringtoDateTime(char* DateString, char* TimeString, ERTCCalender_t* Calender, ERTCTime_t* Time){
	switch(*DateString){
		case 'J':
			if(*(DateString+1) == 'a'){
				//Jan
				Calender->Month = 1;
			}
			else if(*(DateString+2) == 'n'){
				//Jun
				Calender->Month = 6;
			}
			else{
				//July
				Calender->Month = 7;
			}
			break;
		case 'F':
			//Feb
			Calender->Month = 2;
			break;
		case 'M':
			//Mar => 3, May => 5
			Calender->Month = ((*DateString+2) == 'r')?	3:5;
			break;
		case 'A':
			//Apr => 4, Aug => 8
			Calender->Month = (*(DateString+1) == 'p')? 4:8;
			break;
		case 'S':
			//Sep
			Calender->Month = 9;
			break;
		case 'O':
			//Oct
			Calender->Month = 10;
			break;
		case 'N':
			//Nov
			Calender->Month = 11;
			break;
		case 'D':
			//Dec
			Calender->Month = 12;
			break;
		default:
			break;
	}
	Calender->Date = LSTRING_s32ToInt(DateString+4,' ');
	Calender->Year = LSTRING_s32ToInt(DateString+7,' ');

	Time->Hour = LSTRING_s32ToInt(TimeString,':');
	Time->Minute = LSTRING_s32ToInt(TimeString + 3,':');
	Time->Second = LSTRING_s32ToInt(TimeString + 6,':');
}


void HRTC_voidSetOutputMode(ERTC_Config_t* RTC, ERTC_OutputMode_t OutputMode){

	u8 Local_u8Buffer[2] = {0x7,OutputMode};

	MI2C_enuConnect(RTC->I2C,EXTERNAL_RTC_I2C_ADDRESS, I2C_DIRECTION_WRITE);
	MI2C_enuWriteNoStopSynch(RTC->I2C,Local_u8Buffer,2);
	MI2C_enuDisconnect(RTC->I2C);
}





s8 PERTC_s8CompareTime(ERTCCalender_t* CalenderA ,ERTCTime_t* TimeA, ERTCCalender_t* CalenderB ,ERTCTime_t* TimeB){
	s8 Local_s8Comparison = 0;	//Both Equal
	if(CalenderA->Year > CalenderB->Year)
		Local_s8Comparison = 1;
	else if (CalenderA->Year < CalenderB->Year)
		Local_s8Comparison = -1;
	else if (CalenderA->Month > CalenderB->Month)
		Local_s8Comparison = 1;
	else if (CalenderA->Month < CalenderB->Month)
		Local_s8Comparison = -1;
	else if (CalenderA->Date > CalenderB->Date)
		Local_s8Comparison = 1;
	else if (CalenderA->Date < CalenderB->Date)
		Local_s8Comparison = -1;
	else if (TimeA->Hour > TimeB->Hour)
		Local_s8Comparison = 1;
	else if (TimeA->Hour < TimeB->Hour)
		Local_s8Comparison = -1;
	else if (TimeA->Minute > TimeB->Minute)
		Local_s8Comparison = 1;
	else if (TimeA->Minute < TimeB->Minute)
		Local_s8Comparison = -1;
	else if (TimeA->Second > TimeB->Second)
		Local_s8Comparison = 1;
	else if (TimeA->Second < TimeB->Second)
		Local_s8Comparison = -1;
	return Local_s8Comparison;
}

void PERTC_voidRegistersToDateTime(u8 Registers[7], ERTCCalender_t* Calender, ERTCTime_t* Time){

	Time->Second = (Registers[0] & 0xF) + ((Registers[0] & 0x70)>>4)*10;
	Time->Minute = (Registers[1] & 0xF) + ((Registers[1] & 0x70)>>4)*10;
	Time->Hour =  (Registers[2] & 0xF) + ((Registers[2] & 0x30)>>4)*10;

	Calender->Day = Registers[3] & 0x7;
	Calender->Date = (Registers[4] & 0xF) + ((Registers[4] & 0x30)>>4)*10;
	Calender->Month = (Registers[5] & 0xF) + ((Registers[5] & 0x10)>>4)*10;
	Calender->Year = (Registers[6] & 0xF) + ((Registers[6] & 0xF0)>>4)*10  + 2000;
}

void PERTC_voidDateTimeToRegisters(u8 Registers[7], ERTCCalender_t* Calender, ERTCTime_t* Time){

	Registers[0] = ((Time->Second/10) << 4 ) | (Time->Second%10);
	Registers[1] = ((Time->Minute/10) << 4 ) | (Time->Minute%10);
	Registers[2] = ((Time->Hour/10) << 4 ) | (Time->Hour%10);

	Registers[3] = 0;//((Calender->Day));
	Registers[4] = ((Calender->Date/10) << 4) | (Calender->Date % 10);
	Registers[5] = ((Calender->Month/10) << 4) | (Calender->Month % 10);
	Registers[6] = (Calender->Year > 2000)? (((Calender->Year-2000)/10) << 4) | ((Calender->Year-2000) % 10):((Calender->Year/10) << 4) | (Calender->Year % 10) ;
}
