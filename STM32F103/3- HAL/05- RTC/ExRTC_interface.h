/*
 * ExRTC_interface.h
 *
 *  Created on: Apr 19, 2022
 *      Author: 20109
 */

#ifndef DRIVERS_3__HAL_04__RTC_EXRTC_INTERFACE_H_
#define DRIVERS_3__HAL_04__RTC_EXRTC_INTERFACE_H_


typedef enum{
	ERTC_MODE_12_HOURS,
	ERTC_MODE_24_HOURS
}ERTCMode_t;

typedef struct{
	u8 Hour;
	u8 Minute;
	u8 Second;
}ERTCTime_t;

typedef struct{
	u16 Year;
	u8 Month;
	u8 Date;
	u8 Day;
}ERTCCalender_t;

typedef enum{
	ERTC_OUTPUT_MODE_1HZ = 0x10,
	ERTC_OUTPUT_MODE_4096HZ = 0x11,
	ERTC_OUTPUT_MODE_8192HZ = 0x12,
	ERTC_OUTPUT_MODE_32768HZ = 0x13,
	ERTC_OUTPUT_MODE_IDLE_HIGH = 0x00,
	ERTC_OUTPUT_MODE_IDLE_LOW = 0x90
}ERTC_OutputMode_t;

typedef struct{
	I2C_ID_t I2C;
	ERTCMode_t	Mode;
}ERTC_Config_t;


void HRTC_voidInit(ERTC_Config_t* RTC);

void HRTC_voidGetDateTime(ERTC_Config_t* RTC, ERTCCalender_t* Calender, ERTCTime_t* Time);

void HRTC_voidSetDateTime(ERTC_Config_t* RTC, ERTCCalender_t* Calender, ERTCTime_t* Time);

void HRTC_voidStringtoDateTime(char* DateString, char* TimeString, ERTCCalender_t* Calender, ERTCTime_t* Time);

void HRTC_voidSetOutputMode(ERTC_Config_t* RTC, ERTC_OutputMode_t OutputMode);
#endif /* DRIVERS_3__HAL_04__RTC_EXRTC_INTERFACE_H_ */
