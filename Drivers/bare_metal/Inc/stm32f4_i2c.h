/*
 * stm32f4_i2c.h
 *
 *  Created on: Nov 10, 2022
 *      Author: Samet
 */

#ifndef BARE_METAL_I2C_INC_STM32F4_I2C_H_
#define BARE_METAL_I2C_INC_STM32F4_I2C_H_
#include "stm32f4.h"
#include "stdint.h"

void I2C_Master_Write (I2C_Handler *hi2cx, uint8_t deviceAddr, uint16_t memAddr,
		       int mem_addr_size, uint8_t *pData, int pdataSize);

void I2C_Master_Read (I2C_Handler *hi2cx, uint8_t deviceAddr, uint16_t memAddr,
		      int mem_addr_size, uint8_t *pBuffer, int pbufSize);

void I2C_Master_Write_IT (I2C_Handler *hi2cx, uint8_t deviceAddr, uint16_t memAddr, int mem_addr_size, uint8_t *pData, int pdataSize);
void I2C_Master_Read_IT (I2C_Handler *hi2cx, uint8_t deviceAddr, uint16_t memAddr,
			 int mem_addr_size, uint8_t *pBuffer, int pbufSize);
void I2C1_Init (I2C_Handler *hi2cx, I2C_mode mode, uint32_t i2c_freq);
void I2C_IRQ (I2C_Handler *hi2cx);
#endif /* BARE_METAL_I2C_INC_STM32F4_I2C_H_ */

