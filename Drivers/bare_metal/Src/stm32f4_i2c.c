/*
 * stm32f4_i2c.c
 *
 *  Created on: Nov 10, 2022
 *      Author: Samet
 */

#include "stm32f4_i2c.h"
#include "stm32f4.h"
#include "i2c_request.h"

/**
 * @brief  i2c Init function
 * @param  I2C_Handler handle of I2C block
 * @param  I2C mode fast or standard mode
 * @param  I2C frequency
 * @retval none
 */
void I2C1_Init (I2C_Handler *hi2cx, I2C_mode mode, uint32_t i2c_freq)
{

       hi2cx->HW_Block = I2C1A;
       hi2cx->init.freq = i2c_freq;
       hi2cx->init.duty_cycle = DUTY_CYCLE_2;
       hi2cx->State = READY;
       switch (mode)
       {
	      case 0:
		     i2c_conf_sm (hi2cx);
		     break;
	      case 1:
		     i2c_conf_fm (hi2cx);
		     break;
	      default:
		     i2c_conf_sm (hi2cx);
		     break;
       }

}

/**
 * @brief  i2c Write function
 * @param  I2C_Handler handle of I2C block
 * @param  I2C Device Address
 * @param  I2C Register Address
 * @param  I2C Register Address Size
 * @param  Pointer data to write
 * @param  Size of pointer data
 * @retval none
 */
void I2C_Master_Write (I2C_Handler *hi2cx, uint8_t deviceAddr, uint16_t memAddr,
		       int mem_addr_size, uint8_t *pData, int pdataSize)
{
       if (hi2cx->State != READY)
	      {
		     return;
	      }

       hi2cx->slaveAddr = deviceAddr;
       hi2cx->regAddr = memAddr;
       hi2cx->pBuffer = pData;
       hi2cx->TX_Xfer_Size = pdataSize;
       hi2cx->Xfer_Count = 0;
       hi2cx->memSize = mem_addr_size;

       I2C_memWrite_pol (hi2cx);

}

/**
 * @brief  I2C Read	function
 * @param  I2C_Handler handle of I2C block
 * @param  I2C Device Address
 * @param  I2C Register Address
 * @param  I2C Register Address Size
 * @param  Pointer buffer to read
 * @param  Size of pointer buffer
 * @retval none
 */

void I2C_Master_Read (I2C_Handler *hi2cx, uint8_t deviceAddr, uint16_t memAddr,
		      int mem_addr_size, uint8_t *pBuffer, int pbufSize)
{
       if (hi2cx->State != READY)
	      {
		     return;
	      }

       hi2cx->slaveAddr = deviceAddr;

       hi2cx->regAddr = memAddr;

       hi2cx->pBuffer = pBuffer;

       hi2cx->RX_Xfer_Size = pbufSize;

       hi2cx->Xfer_Count = 0;

       hi2cx->memSize = mem_addr_size;

       I2C_memRead_pol (hi2cx);

}

void I2C_Master_Read_IT (I2C_Handler *hi2cx, uint8_t deviceAddr, uint16_t memAddr,
			 int mem_addr_size, uint8_t *pBuffer, int pbufSize)
{
       if (hi2cx->State != READY)
	      {
		     return;
	      }

       hi2cx->slaveAddr = deviceAddr;
       hi2cx->regAddr = memAddr;
       hi2cx->pBuffer = pBuffer;
       hi2cx->RX_Xfer_Size = pbufSize;
       hi2cx->memSize = mem_addr_size;
       hi2cx->Xfer_Count = 0;
       hi2cx->mode = I2C_RX;
       hi2cx->State = IN_RX_MODE;
       generate_start (hi2cx);

       Enable_I2C_Interrrupt (hi2cx);
}

void I2C_Master_Write_IT (I2C_Handler *hi2cx, uint8_t deviceAddr, uint16_t memAddr,
			  int mem_addr_size, uint8_t *pData, int pdataSize)
{
       if (hi2cx->State != READY)
	      {
		     return;
	      }

       hi2cx->slaveAddr = deviceAddr;
       hi2cx->regAddr = memAddr;
       hi2cx->pBuffer = pData;
       hi2cx->TX_Xfer_Size = pdataSize;
       hi2cx->memSize = mem_addr_size;
       hi2cx->Xfer_Count = 0;
       hi2cx->mode = I2C_TX;
       hi2cx->State = IN_TX_MODE;

       generate_start (hi2cx);
       Enable_I2C_Interrrupt (hi2cx);

}

void I2C_IRQ (I2C_Handler *hi2cx)
{
       if (hi2cx->mode == I2C_TX)
	      {
		     i2c_write_seq_IT (hi2cx);
	      }
       else if (hi2cx->mode == I2C_RX)
	      {
		     i2c_read_seq_IT (hi2cx);
	      }
}
