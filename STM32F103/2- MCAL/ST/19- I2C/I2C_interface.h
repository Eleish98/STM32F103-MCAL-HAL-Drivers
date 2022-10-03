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

#ifndef I2C_INTERFACE_H_INCLUDED
#define I2C_INTERFACE_H_INCLUDED

typedef enum{
	
	I2C1,
	I2C2
	
}I2C_ID_t;



typedef enum{
	
	I2C_MODE_I2C,
	I2C_MODE_SMBUS_HOST,
	I2C_MODE_SMBUS_DEVICE
}I2C_Mode_t;

typedef enum{
	
	I2C_ADDRESS_7_BITS,
	I2C_ADDRESS_10_BITS
}I2C_AddressingMode_t;

typedef enum{
	I2C_DUTY_OFF,
	I2C_DUTY_ON
}I2C_Duty_t;

typedef enum{
	
	I2C_CONNECT_OK,
	I2C_CONNECT_NOK
}I2C_Connect_t;

typedef enum{
	I2C_STATUS_INVALID_INSTANCE,
	I2C_STATUS_HEADER_NACK,
	I2C_STATUS_ADDRESS_NACK,
	I2C_STATUS_DATA_SENT_NACK,
	I2C_STATUS_OK,
	I2C_STATUS_ERROR

}I2C_Status_t;

typedef enum{
	I2C_ERROR_OK,
	I2C_ERROR_WRONG_START_STOP,
	I2C_ERROR_ARBITRATION_LOSS,
	I2C_ERROR_OVERRUN,
	I2C_ERROR_PEC,
	I2C_ERROR_TIMEOUT,
	I2C_ERROR_MULTIPLE
}I2C_Error_t;


typedef enum{
	I2C_DIRECTION_WRITE,
	I2C_DIRECTION_READ
}I2C_Direction_t;


void MI2C_voidInit(I2C_ID_t I2C_Instance, I2C_Mode_t I2C_Mode, u8 Copy_u8BusFrequency, u32 Copy_u32I2CFrequency);

void MI2C_voidSetOwnAddress(I2C_ID_t I2C_Instance, u16 Address);

I2C_Error_t MI2C_enuGetError(I2C_ID_t Instance);

I2C_Status_t MI2C_enuMasterWriteSynch(I2C_ID_t I2C_Instance, u16 SlaveAddress, u8* DataBytes, u8 DataLength);

I2C_Status_t MI2C_enuMasterReadSynch(I2C_ID_t I2C_Instance, u16 SlaveAddress, u8* DataBuffer, u8 DataLength);

void MI2C_voidSlaveWriteSynch(I2C_ID_t I2C_Instance, u8* DataBytes, u8 DataLength);

void MI2C_voidSlaveReadSynch(I2C_ID_t I2C_Instance, u8* DataBytes, u8 DataLength);

I2C_Status_t MI2C_enuConnect(I2C_ID_t Instance, u16 SlaveAddress, I2C_Direction_t Direction );

I2C_Status_t MI2C_enuDisconnect(I2C_ID_t Instance);

I2C_Status_t MI2C_enuWriteNoStopSynch(I2C_ID_t Instance, u8* DataBytes, u8 DataLength);

I2C_Status_t MI2C_enuReadNoStopSynch(I2C_ID_t Instance, u8* DataBuffer, u8 DataLength);


#endif


