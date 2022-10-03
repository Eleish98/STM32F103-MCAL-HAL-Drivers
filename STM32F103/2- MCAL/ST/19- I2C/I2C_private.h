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


#ifndef I2C_PRIVATE_H_INCLUDED
#define I2C_PRIVATE_H_INCLUDED

#define I2C1_BASE_ADDRESS				0x40005400
#define I2C2_BASE_ADDRESS				0x40005800

#define I2C_MAX_INSTANCE				I2C2

typedef struct{
	
	volatile u32 CR1	;
	volatile u32 CR2	;
	volatile u32 OAR1	;
	volatile u32 OAR2	;
	volatile u32 DR		;
	volatile u32 SR1	;
	volatile u32 SR2	;
	volatile u32 CCR	;
	volatile u32 TRISE	;
}REG_I2C_t;


#define I2C1_REG					((volatile REG_I2C_t*)(I2C1_BASE_ADDRESS))
#define I2C2_REG					((volatile REG_I2C_t*)(I2C2_BASE_ADDRESS))

volatile REG_I2C_t* I2C_REGS[2] = {I2C1_REG,I2C2_REG};



I2C_Error_t PI2C_enuErrors[2] = {I2C_ERROR_OK,I2C_ERROR_OK};


u8 PI2C_voidWriteBAddressSynch(I2C_ID_t I2C_Instance, u8 Copy_u8Address);

//I2C_Status_t PI2C_enuConnect(I2C_ID_t Instance, u16 SlaveAddress, I2C_Direction_t Direction);

I2C_Error_t PI2C_enuClearError(I2C_ID_t Instance);

#endif

