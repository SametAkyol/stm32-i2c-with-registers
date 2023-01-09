/*
 * i2c_reg.h
 *
 *  Created on: Nov 10, 2022
 *      Author: Samet
 */

#ifndef BARE_METAL_INC_I2C_REG_H_
#define BARE_METAL_INC_I2C_REG_H_
#include "stm32f4.h"



void i2c_enable (I2C_Handler *hi2cx);

I2C_ERROR_e generate_start (I2C_Handler *hi2cx);

I2C_ERROR_e generate_stop (I2C_Handler *hi2cx);

I2C_ERROR_e send_Address (I2C_Handler *hi2cx);

I2C_ERROR_e send_register_address (I2C_Handler *hi2cx);

I2C_ERROR_e i2c_write_seq (I2C_Handler *hi2cx);

I2C_ERROR_e i2c_read_seq (I2C_Handler *hi2cx);


I2C_ERROR_e i2c_write_seq_IT (I2C_Handler *hi2cx);

I2C_ERROR_e i2c_read_seq_IT (I2C_Handler *hi2cx);


void i2c_conf_fm (I2C_Handler *hi2cx);

void i2c_conf_sm (I2C_Handler *hi2cx);

void Enable_I2C_Interrrupt(I2C_Handler *hi2cx);

void Disable_I2C_Interrrupt(I2C_Handler *hi2cx);

#endif /* BARE_METAL_INC_I2C_REG_H_ */
