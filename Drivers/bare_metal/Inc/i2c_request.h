/*
 * i2c_request.h
 *
 *  Created on: 15 Kas 2022
 *      Author: Samet
 */

#ifndef BARE_METAL_INC_I2C_REQUEST_H_
#define BARE_METAL_INC_I2C_REQUEST_H_
#include "stm32f4_i2c.h"
#include "stm32f4.h"
#include "i2c_reg.h"

status I2C_memWrite_pol (I2C_Handler *hi2cx);
status I2C_memRead_pol (I2C_Handler *hi2cx);

status I2C_memWrite_DMA(I2C_Handler *hi2cx);
status I2C_memRead_DMA (I2C_Handler *hi2cx);

status I2C_memWrite_IT(I2C_Handler *hi2cx);

#endif /* BARE_METAL_INC_I2C_REQUEST_H_ */
