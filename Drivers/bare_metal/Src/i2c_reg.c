/*
 * i2c_reg.c
 *
 *  Created on: Nov 10, 2022
 *      Author: Samet
 */
#include "i2c_reg.h"
#include "stm32f4.h"
#include "i2c_command.h"
#include "stm32f4xx_hal.h"

void i2c_conf_sm (I2C_Handler *hi2cx)
{
       hi2cx->HW_Block->CR1 &= ~ CR1_PE;
       hi2cx->HW_Block->CR1 |= CR1_SWRST;
       hi2cx->HW_Block->CR1 &= ~ CR1_SWRST;

       //see datasheet for CCR calculation
       uint32_t CCR, period;
       uint32_t Tpclk = 24; //42mhz period
       period = ((1000000000UL / hi2cx->init.freq)) / 2; //nanosecond period
       CCR = period / Tpclk;

       hi2cx->HW_Block->CCR &= ~(1 << 15);
       hi2cx->HW_Block->CCR |= (CCR << 0);
       hi2cx->HW_Block->CR2 |= 42; //apb1 clock
       hi2cx->HW_Block->TRISE |= 43; //1000/24

       hi2cx->HW_Block->CR1 |= CR1_PE;

}

void i2c_conf_fm (I2C_Handler *hi2cx)
{
       //disable peripheral first
       hi2cx->HW_Block->CR1 &= ~ CR1_PE;
       //reset i2c
       hi2cx->HW_Block->CR1 |= CR1_SWRST;
       hi2cx->HW_Block->CR1 &= ~ CR1_SWRST;

       //enable fast mode
       hi2cx->HW_Block->CCR |= (1 << 15);

       uint32_t CCR = 0;
       uint32_t period = 0;
       uint32_t Tpclk = 23; // 42mhz freq->period
       //see datasheet for CCR calculation
       period = ((1000000000UL / hi2cx->init.freq));

       if (hi2cx->init.duty_cycle == DUTY_CYCLE_16_9)
              {
		     //set duty 16:9
		     hi2cx->HW_Block->CCR |= (1 << 14);

		     CCR = (uint32_t) period / (uint32_t) (25 * Tpclk);
	      }
       else if (hi2cx->init.duty_cycle == DUTY_CYCLE_2)
              {
		     //set duty 2:1
		     hi2cx->HW_Block->CCR &= ~(1 << 14);

		     CCR = (uint32_t) period / (uint32_t) (3 * Tpclk);
	      }

       //set CCR register
       hi2cx->HW_Block->CCR |= (CCR << 0);
       //set PClk1
       hi2cx->HW_Block->CR2 |= 42;
       //see for calculation
       hi2cx->HW_Block->TRISE |= 13; // max SCL rise time / Tpclk
       //enable peripheral
       hi2cx->HW_Block->CR1 |= CR1_PE;
}

flag_status_e wait_flag_set (
			     I2C_HW_t *hw_block, uint32_t flag, uint32_t timeout)
{
       uint32_t tick_Start = HAL_GetTick ();

       while (((HAL_GetTick () - tick_Start) < timeout))
              {
		     if ((hw_block->SR1 & (flag)))
		            {
				   return noError;
			    }
	      }
       return (HAL_GetTick () - tick_Start);
}

I2C_ERROR_e generate_start (I2C_Handler *hi2cx)
{

       /*
        * send start condition??
        *
        *
        */
       hi2cx->HW_Block->CR1 |= (CR1_ACK);
       hi2cx->HW_Block->CR1 |= (CR1_START);

       if (wait_flag_set (hi2cx->HW_Block, SR1_SB, I2C_MAX_DELAY) != noError)
              {
		     return start_timeOut;
	      }
       return OK;

}

I2C_ERROR_e send_Address (I2C_Handler *hi2cx)
{
       /*
        * write slave adress to DR register
        * wait for address send register to set
        * clear address flag
        * if flag not set send stop condition
        */
       hi2cx->HW_Block->DR = hi2cx->slaveAddr;

       if (wait_flag_set (hi2cx->HW_Block, SR1_ADDR, I2C_MAX_DELAY) == noError)
              {
		     uint8_t temp = clear_addr(hi2cx);
		     (void) temp;
		     return OK;

	      }
       generate_stop (hi2cx);
       return address_Mismatched;

}

I2C_ERROR_e send_register_address (I2C_Handler *hi2cx)
{
       if (hi2cx->memSize == _8BIT_MEM_ADDR_)
              {
		     //send 8bit register adress
		     wait_flag_set (hi2cx->HW_Block, SR1_TxE, I2C_MAX_DELAY);

		     hi2cx->HW_Block->DR = hi2cx->regAddr;
		     wait_flag_set (hi2cx->HW_Block, SR1_BTF, I2C_MAX_DELAY);

	      }

       else
              {
		     /*
		      * if memory size is 16bit ,send most significant byte first
		      * then send ,least significant  byte
		      */
		     wait_flag_set (hi2cx->HW_Block, SR1_TxE, I2C_MAX_DELAY);

		     hi2cx->HW_Block->DR = (uint8_t) MSB(hi2cx->regAddr);
		     wait_flag_set (hi2cx->HW_Block, SR1_TxE, I2C_MAX_DELAY);

		     hi2cx->HW_Block->DR = (uint8_t) LSB(hi2cx->regAddr);
		     wait_flag_set (hi2cx->HW_Block, SR1_BTF, I2C_MAX_DELAY);

	      }
       return OK;
}

I2C_ERROR_e i2c_write_seq (I2C_Handler *hi2cx)
{

       while (hi2cx->Xfer_Count < hi2cx->TX_Xfer_Size)
              {
		     wait_flag_set (hi2cx->HW_Block, SR1_TxE, I2C_MAX_DELAY);

		     hi2cx->HW_Block->DR = hi2cx->pBuffer[hi2cx->Xfer_Count++];
		     wait_flag_set (hi2cx->HW_Block, SR1_BTF, I2C_MAX_DELAY);

	      }
       generate_stop (hi2cx);
       return OK;
}

I2C_ERROR_e i2c_read_seq (I2C_Handler *hi2cx)
{
       generate_start (hi2cx);

       if (hi2cx->RX_Xfer_Size == 1)
              {
		     hi2cx->HW_Block->DR = hi2cx->slaveAddr + 1;
		     wait_flag_set (hi2cx->HW_Block, SR1_ADDR, I2C_MAX_DELAY);

		     hi2cx->HW_Block->CR1 &= ~(CR1_ACK);
		     uint8_t temp = clear_addr(hi2cx);
		     (void) temp;
		     generate_stop (hi2cx);
		     hi2cx->pBuffer[0] = hi2cx->HW_Block->DR;
		     wait_flag_set (hi2cx->HW_Block, SR1_RxE, I2C_MAX_DELAY);

	      }

       else if (hi2cx->RX_Xfer_Size == 2)
              {
		     hi2cx->HW_Block->DR = hi2cx->slaveAddr + 1;
		     wait_flag_set (hi2cx->HW_Block, SR1_ADDR, I2C_MAX_DELAY);
		     hi2cx->HW_Block->CR1 &= ~(CR1_ACK);
		     hi2cx->HW_Block->CR1 |= (CR1_POS);
		     uint8_t temp = clear_addr(hi2cx);
		     (void) temp;
		     wait_flag_set (hi2cx->HW_Block, SR1_BTF, I2C_MAX_DELAY);
		     generate_stop (hi2cx);
		     hi2cx->pBuffer[0] = hi2cx->HW_Block->DR;
		     wait_flag_set (hi2cx->HW_Block, SR1_RxE, I2C_MAX_DELAY);
		     hi2cx->pBuffer[1] = hi2cx->HW_Block->DR;
	      }

       else
              {

		     hi2cx->HW_Block->DR = hi2cx->slaveAddr + 1;
		     wait_flag_set (hi2cx->HW_Block, SR1_ADDR, I2C_MAX_DELAY);
		     uint8_t temp = clear_addr(hi2cx);
		     (void) temp;
		     while (hi2cx->RX_Xfer_Size - hi2cx->Xfer_Count > 3)
		            {

				   wait_flag_set (hi2cx->HW_Block, SR1_RxE, 10);
				   hi2cx->pBuffer[hi2cx->Xfer_Count++] =
						 hi2cx->HW_Block->DR;
				   hi2cx->HW_Block->CR1 |= (CR1_ACK);

			    }

		     wait_flag_set (hi2cx->HW_Block, SR1_BTF, I2C_MAX_DELAY);

		     hi2cx->HW_Block->CR1 &= ~(CR1_ACK);

		     wait_flag_set (hi2cx->HW_Block, SR1_RxE, I2C_MAX_DELAY);
		     hi2cx->pBuffer[hi2cx->Xfer_Count++] = hi2cx->HW_Block->DR;

		     wait_flag_set (hi2cx->HW_Block, SR1_BTF, I2C_MAX_DELAY);

		     generate_stop (hi2cx);
		     wait_flag_set (hi2cx->HW_Block, SR1_RxE, I2C_MAX_DELAY);
		     hi2cx->pBuffer[hi2cx->Xfer_Count++] = hi2cx->HW_Block->DR;

		     wait_flag_set (hi2cx->HW_Block, SR1_RxE, I2C_MAX_DELAY);
		     hi2cx->pBuffer[hi2cx->Xfer_Count] = hi2cx->HW_Block->DR;
	      }
       hi2cx->State = READY;
       return OK;
}

I2C_ERROR_e i2c_write_seq_IT (I2C_Handler *hi2cx)
{
       if (CHECK_FLAG(hi2cx,CR2,CR2_ITEVTEN) && CHECK_FLAG(hi2cx, SR1, SR1_SB))
              {
		     I2C_Master_Start_IT (hi2cx);
	      }
       if (CHECK_FLAG(hi2cx,CR2,CR2_ITEVTEN) && CHECK_FLAG(hi2cx, SR1, SR1_ADDR))
              {
		     I2C_Master_ADDR_IT (hi2cx);
	      }

       if (CHECK_FLAG(hi2cx,CR2,CR2_ITEVTEN) && CHECK_FLAG(hi2cx, SR1, SR1_BTF))
              {
		     if (CHECK_FLAG(hi2cx, SR1, SR1_TxE))
		            {
				   if (hi2cx->Xfer_Count >= hi2cx->TX_Xfer_Size)
				          {
						 I2C_TX_FINISH (hi2cx);
					  }
			    }
	      }

       if (CHECK_FLAG(hi2cx, CR2,CR2_ITEVTEN) && CHECK_FLAG(hi2cx, CR2, CR2_ITBUFEN) && CHECK_FLAG(hi2cx, SR1, SR1_TxE))
              {
		     I2C_Master_TXE_IT (hi2cx);

		     if (hi2cx->State == REG_ADDRESS_PHASE)
		            {

				   I2C_Master_REGPHASE_IT (hi2cx);

			    }
	      }
       return 1;
}

I2C_ERROR_e i2c_read_seq_IT (I2C_Handler *hi2cx)
{
       if (CHECK_FLAG(hi2cx,CR2,CR2_ITEVTEN) && CHECK_FLAG(hi2cx, SR1, SR1_SB))
              {

		     if (hi2cx->Sr == true && hi2cx->State == IN_RX_MODE)
		            {
				   hi2cx->slaveAddr += 1;
				   I2C_Master_Start_IT (hi2cx);

			    }
		     else
		            {
				   I2C_Master_Start_IT (hi2cx);
			    }
	      }
       if (CHECK_FLAG(hi2cx,CR2,CR2_ITEVTEN) && CHECK_FLAG(hi2cx, SR1, SR1_ADDR))
              {

		     if (hi2cx->Sr == true && hi2cx->State == IN_RX_MODE)
		            {

				   if (hi2cx->RX_Xfer_Size == 1 )
				          {
						 hi2cx->HW_Block->CR1 &= ~(CR1_ACK);
					  }
				   if (hi2cx->RX_Xfer_Size == 2)
				          {
						 hi2cx->HW_Block->CR1 |= (CR1_POS);
						 hi2cx->HW_Block->CR1 &= ~(CR1_ACK);
					  }
				   uint8_t temp = clear_addr(hi2cx);
				   (void) temp;
				   hi2cx->Sr = false;

			    }
		     else
		            {
				   I2C_Master_ADDR_IT (hi2cx);
			    }
	      }

       if (CHECK_FLAG(hi2cx, CR2,CR2_ITEVTEN) && CHECK_FLAG(hi2cx, CR2, CR2_ITBUFEN) && CHECK_FLAG(hi2cx, SR1, SR1_TxE))
              {
		     if (hi2cx->State == REG_ADDRESS_PHASE)
		            {

				   I2C_Master_REGPHASE_IT (hi2cx);

			    }

	      }
       if (CHECK_FLAG(hi2cx, CR2,CR2_ITEVTEN) && CHECK_FLAG(hi2cx, SR1, SR1_BTF))
              {
		     if (hi2cx->State == IN_RX_MODE)
		            {
				   generate_start (hi2cx);
				   hi2cx->Sr = true;

			    }
	      }

       if (CHECK_FLAG(hi2cx, CR2,CR2_ITEVTEN) && CHECK_FLAG(hi2cx, CR2, CR2_ITBUFEN) && CHECK_FLAG(hi2cx, SR1, SR1_RxE))
              {

		     I2C_Master_RXE_IT (hi2cx);

	      }
}

