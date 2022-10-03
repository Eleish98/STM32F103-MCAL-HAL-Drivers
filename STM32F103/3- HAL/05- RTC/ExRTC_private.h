/*
 * ExRTC_private.h
 *
 *  Created on: Apr 19, 2022
 *      Author: 20109
 */

#ifndef DRIVERS_3__HAL_04__RTC_EXRTC_PRIVATE_H_
#define DRIVERS_3__HAL_04__RTC_EXRTC_PRIVATE_H_

#define EXTERNAL_RTC_I2C_ADDRESS				((u8)(0x68))


s8 PERTC_s8CompareTime(ERTCCalender_t* CalenderA ,ERTCTime_t* TimeA, ERTCCalender_t* CalenderB ,ERTCTime_t* TimeB);

void PERTC_voidRegistersToDateTime(u8 Registers[7], ERTCCalender_t* Calender, ERTCTime_t* Time);

void PERTC_voidDateTimeToRegisters(u8 Registers[7], ERTCCalender_t* Calender, ERTCTime_t* Time);

typedef struct{

}RTC_Reg_t;

#endif /* DRIVERS_3__HAL_04__RTC_EXRTC_PRIVATE_H_ */
