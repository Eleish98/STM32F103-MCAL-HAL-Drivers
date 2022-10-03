/************************************************/
/*	Author		:		Mohamed G. ELeish		*/
/*	Date		:		12 March 2022		*/
/*	Version		:		V01						*/
/************************************************/

/************************************************/
/************************************************/
/*****					LOG					*****/
/*	12/3/2022:	File Created					*/
/************************************************/
/************************************************/

#include "1- LIB/STD_TYPES.h"
#include "1- LIB/BIT_MATH.h"

#include "I2C_interface.h"
#include "I2C_private.h"

void MI2C_voidInit(I2C_ID_t I2C_Instance, I2C_Mode_t I2C_Mode, u8 Copy_u8BusFrequency, u32 Copy_u32I2CFrequency){

	u16 Local_u16CCRBits = 0;
	if (I2C_Instance > I2C_MAX_INSTANCE)
		return;
	CLR_BIT(I2C_REGS[I2C_Instance]->CR1,0);
	switch(I2C_Mode){
		case I2C_MODE_I2C:
			I2C_REGS[I2C_Instance]->CR1 &= ~ (0x05);
			break;
		case I2C_MODE_SMBUS_HOST:
			I2C_REGS[I2C_Instance]->CR1 |= 0x5;
			break;
		case I2C_MODE_SMBUS_DEVICE:
			I2C_REGS[I2C_Instance]->CR1 |= 0x02;
		default:
			return;
	}
	if (Copy_u8BusFrequency < 2 || Copy_u8BusFrequency > 50)
		return;
	I2C_REGS[I2C_Instance]->CR2 = Copy_u8BusFrequency;

	if (I2C_Mode != I2C_MODE_I2C || Copy_u32I2CFrequency <= 100000){
		//Standard Mode
		CLR_BIT(I2C_REGS[I2C_Instance]->CCR,15);
		Local_u16CCRBits = (u16)((1000000UL * Copy_u8BusFrequency)/(2*Copy_u32I2CFrequency)) ;
		if (Local_u16CCRBits < 4 )
			Local_u16CCRBits = 4;
		I2C_REGS[I2C_Instance]->TRISE = Copy_u8BusFrequency+1;
	}
	else{
		//Fast Mode
		SET_BIT(I2C_REGS[I2C_Instance]->CCR,15);
		if (Copy_u32I2CFrequency < 400000){
			Local_u16CCRBits = (u16)((1000000UL * Copy_u8BusFrequency)/(3*Copy_u32I2CFrequency)) ;
			I2C_REGS[I2C_Instance]->TRISE = (u16)(Copy_u8BusFrequency * 3UL /10UL )+1;
		}

		else{
			if ((Copy_u32I2CFrequency == 400000 && (Copy_u8BusFrequency % 10 != 0)))
				return;
			Local_u16CCRBits = (u16)((1000000UL * Copy_u8BusFrequency)/(25*Copy_u32I2CFrequency)) ;
			I2C_REGS[I2C_Instance]->TRISE = (u16)(Copy_u8BusFrequency * 3UL /10UL )+1;
			SET_BIT(I2C_REGS[I2C_Instance]->CCR,14);
		}

	}
	I2C_REGS[I2C_Instance]->CCR |= ((u16)Local_u16CCRBits) & 0xfff;


	SET_BIT(I2C_REGS[I2C_Instance]->CR1,0);
}

void MI2C_voidSetOwnAddress(I2C_ID_t I2C_Instance, u16 Address){


}

I2C_Status_t MI2C_enuConnect(I2C_ID_t I2C_Instance, u16 SlaveAddress, I2C_Direction_t Direction){

	u8 Local_u8DataReg = 0;
	I2C_Status_t Local_enuStatus =I2C_STATUS_OK;
	if (I2C_Instance > I2C_MAX_INSTANCE)
		Local_enuStatus = I2C_STATUS_INVALID_INSTANCE;

	//Check for Busy
	while(CHK_BIT(I2C_REGS[I2C_Instance]->SR1, 1));

	//Address Mode
	if(SlaveAddress > 127)
		SET_BIT(I2C_REGS[I2C_Instance]->OAR1,15);

	//Generate Start Bit
	CLR_BIT(I2C_REGS[I2C_Instance]->CR1,8);
	SET_BIT(I2C_REGS[I2C_Instance]->CR1,8);

	//Check for SB = 1, wait for start condition generation
	while(!CHK_BIT(I2C_REGS[I2C_Instance]->SR1,0));

	//7-bit address or 10-bit address mode?
	if(CHK_BIT(I2C_REGS[I2C_Instance]->OAR1,15)){
		//10-bit address Mode

		//Header with 2 most significant bits of the address and write command
		Local_u8DataReg = 0xF0 | ((SlaveAddress>>7) & 6) | Direction;
		if(PI2C_voidWriteBAddressSynch(I2C_Instance,Local_u8DataReg) != 0){
			// Not Ack
			Local_enuStatus = I2C_STATUS_HEADER_NACK;
		}
		else{
			if(PI2C_voidWriteBAddressSynch(I2C_Instance,SlaveAddress & 0xFF) != 0){
				// Not Ack
				Local_enuStatus = I2C_STATUS_ADDRESS_NACK;
			}
		}
	}
	else{
		//7-bit address Mode
		Local_u8DataReg = (SlaveAddress<<1) | Direction;
		if(PI2C_voidWriteBAddressSynch(I2C_Instance,Local_u8DataReg) != 0){
			// Not Ack
			Local_enuStatus = I2C_STATUS_ADDRESS_NACK;
		}
	}
	//dummy read of SR2
	(void)I2C_REGS[I2C_Instance]->SR2;


	return Local_enuStatus;

}

I2C_Status_t MI2C_enuDisconnect(I2C_ID_t I2C_Instance){
	//Sends Stop Condition

	I2C_Status_t Local_enuStatus = I2C_STATUS_OK;

	//Clear ACK Bit
	CLR_BIT(I2C_REGS[I2C_Instance]->CR1,10);
	//Generate Stop Condition
	SET_BIT(I2C_REGS[I2C_Instance]->CR1,9);

	//wait for stop condition is generated
	while(CHK_BIT(I2C_REGS[I2C_Instance]->SR1, 1));

	if(CHK_BIT(I2C_REGS[I2C_Instance]->SR1,4)){
		I2C_REGS[I2C_Instance]->CR1 = I2C_REGS[I2C_Instance]->CR1;
		Local_enuStatus = I2C_STATUS_OK;
	}
	else{
		Local_enuStatus = I2C_STATUS_ERROR;
	}
	return Local_enuStatus;
}

I2C_Status_t MI2C_enuWriteNoStopSynch(I2C_ID_t I2C_Instance, u8* DataBytes, u8 DataLength){

	I2C_Status_t Local_enuStatus = I2C_STATUS_OK;
		if (Local_enuStatus == I2C_STATUS_OK){
			while(0 < DataLength--){
				//write data byte into DR
				I2C_REGS[I2C_Instance]->DR = *DataBytes++;

				//wait for Tx buffer empty
				while(!CHK_BIT(I2C_REGS[I2C_Instance]->SR1,7) && !(CHK_BIT(I2C_REGS[I2C_Instance]->SR1,2)) && !CHK_BIT(I2C_REGS[I2C_Instance]->SR1,10));

				if(CHK_BIT(I2C_REGS[I2C_Instance]->SR1,10)){
					//Not Ack Received from Slave
					Local_enuStatus = I2C_STATUS_DATA_SENT_NACK;
					break;
				}
			}

		}
		return Local_enuStatus;
}

I2C_Status_t MI2C_enuReadNoStopSynch(I2C_ID_t I2C_Instance, u8* DataBuffer, u8 DataLength){
	u8 Local_u8DataBufferIdx = 0;
	I2C_Error_t Local_enuError = I2C_ERROR_OK;
	I2C_Status_t Local_enuStatus = I2C_STATUS_OK;
	SET_BIT(I2C_REGS[I2C_Instance]->CR1,10);
	if(Local_enuStatus == I2C_STATUS_OK){

		while(DataLength-- > 0){
			if(DataLength == 0){
				//Clear ACK Bit
				CLR_BIT(I2C_REGS[I2C_Instance]->CR1,10);

			}
			//wait for receiving
			while(!CHK_BIT(I2C_REGS[I2C_Instance]->SR1,6) &&/*OVR*/ !CHK_BIT(I2C_REGS[I2C_Instance]->SR1,11) &&/*Timeout*/!CHK_BIT(I2C_REGS[I2C_Instance]->SR1,14));

			//Check for errors
			Local_enuError = PI2C_enuClearError(I2C1);

			//Read Data Byte after receiving
			DataBuffer[Local_u8DataBufferIdx++] = I2C_REGS[I2C_Instance]->DR;
		}

	}

	if(Local_enuError != I2C_ERROR_OK)
		Local_enuStatus = I2C_STATUS_ERROR;

	return Local_enuStatus;
}


//void MI2C_voidWriteByteSynch(I2C_ID_t I2C_Instance, u8 DataByte){
//
//}
//
//u8 MI2C_u8ReadByteSynch(I2C_ID_t I2C_Instance){
//	u8 Local_u8ReadData = 0;
//
//	return Local_u8ReadData;
//
//}

I2C_Status_t MI2C_enuMasterWriteSynch(I2C_ID_t I2C_Instance, u16 SlaveAddress, u8* DataBytes, u8 DataLength){

	I2C_Status_t Local_enuStatus = MI2C_enuConnect(I2C_Instance,SlaveAddress,I2C_DIRECTION_WRITE);
	if (Local_enuStatus == I2C_STATUS_OK){
		while(0 < DataLength--){
			//write data byte into DR
			I2C_REGS[I2C_Instance]->DR = *DataBytes++;

			//wait for Tx buffer empty
			while(!CHK_BIT(I2C_REGS[I2C_Instance]->SR1,7) && !(CHK_BIT(I2C_REGS[I2C_Instance]->SR1,2)) && !CHK_BIT(I2C_REGS[I2C_Instance]->SR1,10));

			if(CHK_BIT(I2C_REGS[I2C_Instance]->SR1,10)){
				//Not Ack Received from Slave
				Local_enuStatus = I2C_STATUS_DATA_SENT_NACK;
				break;
			}
		}
		//Generate Stop Condition
		SET_BIT(I2C_REGS[I2C_Instance]->CR1,9);

		//wait for stop condition is generated
		while(CHK_BIT(I2C_REGS[I2C_Instance]->SR1, 1));

	}
	return Local_enuStatus;
}

I2C_Status_t MI2C_enuMasterReadSynch(I2C_ID_t I2C_Instance, u16 SlaveAddress, u8* DataBuffer, u8 DataLength){

	u8 Local_u8DataBufferIdx = 0;
	I2C_Error_t Local_enuError = I2C_ERROR_OK;
	I2C_Status_t Local_enuStatus = MI2C_enuConnect(I2C_Instance,SlaveAddress,I2C_DIRECTION_READ);
	SET_BIT(I2C_REGS[I2C_Instance]->CR1,10);
	if(Local_enuStatus == I2C_STATUS_OK){

		while(DataLength-- > 0){
			if(DataLength == 0){
				//Clear ACK Bit
				CLR_BIT(I2C_REGS[I2C_Instance]->CR1,10);
				//Generate Stop Condition
				SET_BIT(I2C_REGS[I2C_Instance]->CR1,9);

				//wait for stop condition is generated
				while(CHK_BIT(I2C_REGS[I2C_Instance]->SR1, 1));
			}
			//wait for receiving
			while(!CHK_BIT(I2C_REGS[I2C_Instance]->SR1,6) &&/*OVR*/ !CHK_BIT(I2C_REGS[I2C_Instance]->SR1,11) &&/*Timeout*/!CHK_BIT(I2C_REGS[I2C_Instance]->SR1,14));

			//Check for errors
			Local_enuError = PI2C_enuClearError(I2C1);

			//Read Data Byte after receiving
			DataBuffer[Local_u8DataBufferIdx++] = I2C_REGS[I2C_Instance]->DR;
		}

	}

	if(Local_enuError != I2C_ERROR_OK)
		Local_enuStatus = I2C_STATUS_ERROR;

	return Local_enuStatus;
}
void MI2C_voidSlaveWriteSynch(I2C_ID_t I2C_Instance, u8* DataBytes, u8 DataLength){


}

void MI2C_voidSlaveReadSynch(I2C_ID_t I2C_Instance, u8* DataBytes, u8 DataLength){

	//return 0;

}

u8 PI2C_voidWriteBAddressSynch(I2C_ID_t I2C_Instance, u8 Copy_u8Address){

	//Address Transmission status
	u8 Status = 0;

	//Write SlaveAddress to Data Register
	I2C_REGS[I2C_Instance]->DR = Copy_u8Address & 0xFF;

	//Wait for Address Transmission
	while(!CHK_BIT(I2C_REGS[I2C_Instance]->SR1,1) && !CHK_BIT(I2C_REGS[I2C_Instance]->SR1,10));

	if(CHK_BIT(I2C_REGS[I2C_Instance]->SR1,10)){
		//Not OK
		CLR_BIT(I2C_REGS[I2C_Instance]->SR1,10);
		Status = 1;
	}
	else
	{
		//OK
		Status = 0;
	}

	return Status;
}

I2C_Error_t PI2C_enuClearError(I2C_ID_t Instance){

	I2C_Error_t Local_enuState = I2C_ERROR_OK;

	//Start Checking for Errors
	//Wrong Start Stop Condition
	if(CHK_BIT(I2C_REGS[Instance]->SR1,8)){
		Local_enuState = I2C_ERROR_WRONG_START_STOP;
		//Clear Error
		CLR_BIT(I2C_REGS[Instance]->SR1,8);
	}
	//Arbitration loss
	if(CHK_BIT(I2C_REGS[Instance]->SR1,9)){
		Local_enuState = (Local_enuState == I2C_ERROR_OK)? I2C_ERROR_ARBITRATION_LOSS:I2C_ERROR_MULTIPLE;
		//Clear Error
		CLR_BIT(I2C_REGS[Instance]->SR1,9);
	}
	//Over Run
	if(CHK_BIT(I2C_REGS[Instance]->SR1,11)){
		Local_enuState = (Local_enuState == I2C_ERROR_OK)? I2C_ERROR_OVERRUN:I2C_ERROR_MULTIPLE;
		//Clear Error
		CLR_BIT(I2C_REGS[Instance]->SR1,11);
	}
	//PEC
	if(CHK_BIT(I2C_REGS[Instance]->SR1,12)){
		Local_enuState = (Local_enuState == I2C_ERROR_OK)? I2C_ERROR_PEC:I2C_ERROR_MULTIPLE;
		//Clear Error
		CLR_BIT(I2C_REGS[Instance]->SR1,12);
	}
	//Timeout
	if(CHK_BIT(I2C_REGS[Instance]->SR1,14)){
		Local_enuState = (Local_enuState == I2C_ERROR_OK)? I2C_ERROR_TIMEOUT:I2C_ERROR_MULTIPLE;
		//Clear Error
		CLR_BIT(I2C_REGS[Instance]->SR1,14);
	}

	PI2C_enuErrors[Instance] = Local_enuState;

	return Local_enuState;
}

I2C_Error_t MI2C_enuGetError(I2C_ID_t Instance){

	if (Instance > I2C_MAX_INSTANCE)
		return I2C_ERROR_MULTIPLE;
	return PI2C_enuErrors[Instance];
}
