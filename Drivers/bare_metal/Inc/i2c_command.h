/*
 * i2c_command.h
 *
 *  Created on: 9 Ara 2022
 *      Author: Samet
 */

#ifndef BARE_METAL_INC_I2C_COMMAND_H_
#define BARE_METAL_INC_I2C_COMMAND_H_
#include "stm32f4.h"
void i2c_enable (I2C_Handler *hi2cx);

I2C_ERROR_e generate_stop (I2C_Handler *hi2cx);

void Enable_I2C_Interrrupt (I2C_Handler *hi2cx);

void Disable_I2C_Interrrupt (I2C_Handler *hi2cx);

void I2C_Master_Start_IT (I2C_Handler *hi2cx);

void I2C_Master_ADDR_IT (I2C_Handler *hi2cx);

void I2C_Master_REGPHASE_IT (I2C_Handler *hi2cx);

void I2C_Master_TXE_IT (I2C_Handler *hi2cx);

void I2C_Master_RXE_IT (I2C_Handler *hi2cx);

void I2C_TX_FINISH (I2C_Handler *hi2cx);


void I2C_Master_BTf_IT (I2C_Handler *hi2cx);
#endif /* BARE_METAL_INC_I2C_COMMAND_H_ */
