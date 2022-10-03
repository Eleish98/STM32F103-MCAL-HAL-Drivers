/*
 * String_program.c
 *
 *  Created on: Apr 20, 2022
 *      Author: 20109
 */


#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "String_interface.h"


s32 LSTRING_s32ToInt(char* String, char Seperator){

	s32 Local_s32Value = 0;
	s8 Local_s8NegativeFlag = 1;

	if (Local_s8NegativeFlag == -1 ){
		String++;
		Local_s8NegativeFlag = -1;
	}
	while(*String != 0 && *String != Seperator){
		Local_s32Value = Local_s32Value*10 + *String - '0';
		String++;
	}
	return Local_s32Value*Local_s8NegativeFlag;

}

u32 LSTRING_GetStringLength(char* String){
	u32 Local_u32Length = 0;
	while(*String++)
		Local_u32Length++;
	return Local_u32Length;
}
