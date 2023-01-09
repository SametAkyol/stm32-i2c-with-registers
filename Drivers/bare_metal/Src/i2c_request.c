/*
 * i2c_request.c
 *
 *  Created on: 15 Kas 2022
 *      Author: Samet
 */
#include "stm32f4.h"
#include "i2c_reg.h"
#include "i2c_request.h"
#include <stddef.h>
status I2C_memWrite_pol (I2C_Handler *hi2cx)
{
       generate_start (hi2cx);
       if (send_Address (hi2cx) == address_Mismatched)
	      {

		     return NotOk;
	      }
       send_register_address (hi2cx);
       i2c_write_seq (hi2cx);
       return Ok;
}

status I2C_memRead_pol (I2C_Handler *hi2cx)
{
       generate_start (hi2cx);
       if (send_Address (hi2cx) == address_Mismatched)
	      {

		     return NotOk;
	      }
       send_register_address (hi2cx);
       i2c_read_seq (hi2cx);
       return Ok;
}

status I2C_memWrite_DMA (I2C_Handler *hi2cx)
{
       return 0;

}
status I2C_memRead_DMA (I2C_Handler *hi2cx)
{
       return 0;
}

status I2C_memWrite_IT (I2C_Handler *hi2cx)
{

       i2c_write_seq_IT (hi2cx);
       return 0;
}

status I2C_memRead_IT (I2C_Handler *hi2cx)
{
       i2c_read_seq_IT(hi2cx);
       return 0;
}
