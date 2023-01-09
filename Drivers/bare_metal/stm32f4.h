/*
 * stm32f4.h
 *
 *  Created on: Nov 10, 2022
 *      Author: Samet
 */

#ifndef BARE_METAL_I2C_STM32F4_H_
#define BARE_METAL_I2C_STM32F4_H_
#include "stdint.h"
#include "stdbool.h"
#define _REG_ volatile
#define clear_addr(HANDLE)   (HANDLE->HW_Block->SR1 | HANDLE->HW_Block->SR2)
#define CHECK_FLAG(Handle,REG,FLAG) (Handle->HW_Block->REG & FLAG)
#define dummy(temp)  ((void)temp)
#define CLEAR(REG,BIT)  (REG &= ~(BIT))
#define set(REG,BIT)	(REG |= (BIT))
#define I2C_MAX_DELAY 5
#define IN_TX_MODE 0
#define IN_RX_MODE 1
#define READY	   2
#define BUSY	3
#define I2C_TX 0
#define I2C_RX 1


#define I2C_BTF_IT 1
#define REG_ADDRESS_PHASE 2
#define ADDRESS_PHASE_LSB 0
#define ADDRESS_PHASE_MSB 1
#define ADDRESS_PHASE_None 2
#define Transfer_INIS 3
#define  CR1_PE 	(1<<0)
#define  CR1_START	(1<<8)
#define  CR1_STOP	(1<<9)
#define  CR1_ACK	(1<<10)
#define  CR1_POS	(1<<11)
#define  CR1_SWRST	(1<<15)

#define  CR2_ITERREN	(1<<8)
#define  CR2_ITEVTEN	(1<<9)
#define  CR2_ITBUFEN	(1<<10)

#define  SR1_SB		(1<<0)
#define  SR1_ADDR	(1<<1)
#define  SR1_TxE	(1<<7)
#define  SR1_RxE	(1<<6)
#define  SR1_BTF	(1<<2)

#define _8BIT_MEM_ADDR_ 1
#define _16BIT_MEM_ADDR_ 2

#define MSB(address) ((address>>8) & 0xFF)
#define LSB(address) ((address) & 0xFF)

#define I2C2_PERIPHERAL_ADDRESS (0x40005800UL)
#define I2C1_PERIPHERAL_ADDRESS (0x40005400UL)
#define I2C1A ((I2C_HW_t*)I2C1_PERIPHERAL_ADDRESS)
#define I2C2A ((I2C_HW_t*)I2C2_PERIPHERAL_ADDRESS)

#define DUTY_CYCLE_2 0
#define DUTY_CYCLE_16_9 1

typedef struct _I2C_
{
       _REG_ uint32_t CR1;
       _REG_ uint32_t CR2;
       _REG_ uint32_t OAR1;
       _REG_ uint32_t OAR2;
       _REG_ uint32_t DR;
       _REG_ uint32_t SR1;
       _REG_ uint32_t SR2;
       _REG_ uint32_t CCR;
       _REG_ uint32_t TRISE;
       _REG_ uint32_t FLTR;
} I2C_HW_t;
typedef struct rr
{
       uint32_t freq;
       int duty_cycle;

} I2C_Init_t;

typedef enum stat
{
       Ok, NotOk

} status;

typedef struct I2C_Handle
{
       I2C_HW_t *HW_Block;
       I2C_Init_t init;
       uint8_t slaveAddr;
       uint16_t regAddr;
       uint8_t *pBuffer;
       uint32_t TX_Xfer_Size;
       uint32_t RX_Xfer_Size;
       uint32_t Xfer_Count;
       uint32_t memSize;
       uint32_t State;
       uint32_t AddressPhase;
       uint32_t mode;
       bool Sr;


} I2C_Handler;

typedef enum s
{
       noError,
       start_timeOut,
       addr_flag_notset,
       btf_notset,
       TxE_notset,
       RxE_notset
} flag_status_e;

typedef enum err
{
       OK, noStart, address_Mismatched, write_Error, read_Error, noStop
} I2C_ERROR_e;

typedef enum freq
{
       normalmode = 0, fastmode = 1
} I2C_mode;
#endif /* BARE_METAL_I2C_STM32F4_H_ */
