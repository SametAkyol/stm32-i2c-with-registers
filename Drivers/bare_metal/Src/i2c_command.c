/*
 * i2c_command.c
 *
 *  Created on: 9 Ara 2022
 *      Author: Samet
 */
#include "i2c_command.h"
#include "stm32f4.h"
#include <stddef.h>
void I2C_Master_Start_IT (I2C_Handler *hi2cx)
{
       uint8_t temp = hi2cx->HW_Block->SR1;
       (void) temp;
       hi2cx->HW_Block->DR = hi2cx->slaveAddr;
}

void I2C_Master_ADDR_IT (I2C_Handler *hi2cx)
{
       uint8_t temp = clear_addr(hi2cx);
       (void) temp;
       hi2cx->State = REG_ADDRESS_PHASE;
}

void I2C_Master_REGPHASE_IT (I2C_Handler *hi2cx)
{
       if (hi2cx->memSize == _8BIT_MEM_ADDR_)
              {
		     hi2cx->HW_Block->DR = hi2cx->regAddr;
		     if (hi2cx->mode != I2C_RX)
		            {
				   hi2cx->State = IN_TX_MODE;
			    }
		     else
		            {
				   hi2cx->State = IN_RX_MODE;
			    }
	      }

       if (hi2cx->memSize == _16BIT_MEM_ADDR_)
              {
		     hi2cx->AddressPhase = ADDRESS_PHASE_MSB;

		     if (hi2cx->AddressPhase == ADDRESS_PHASE_MSB)
		            {
				   hi2cx->HW_Block->DR = (uint8_t) MSB(hi2cx->regAddr);
				   hi2cx->AddressPhase = ADDRESS_PHASE_LSB;
			    }

		     if (hi2cx->AddressPhase == ADDRESS_PHASE_LSB)
		            {
				   hi2cx->HW_Block->DR = (uint8_t) LSB(hi2cx->regAddr);

				   hi2cx->AddressPhase = ADDRESS_PHASE_None;
				   if (hi2cx->mode != I2C_RX)
				          {
						 hi2cx->State = IN_TX_MODE;
					  }
				   else
				          {
						 hi2cx->State = IN_RX_MODE;
					  }

			    }
	      }
}

void I2C_Master_TXE_IT (I2C_Handler *hi2cx)
{
       if (hi2cx->State == IN_TX_MODE && hi2cx->Xfer_Count < hi2cx->TX_Xfer_Size)
              {
		     hi2cx->HW_Block->DR = hi2cx->pBuffer[hi2cx->Xfer_Count++];
	      }
}

void I2C_Master_RXE_IT (I2C_Handler *hi2cx)
{

       if (hi2cx->mode == I2C_RX && hi2cx->RX_Xfer_Size == 1)
              {
		     generate_stop (hi2cx);
		     Disable_I2C_Interrrupt (hi2cx);
		     hi2cx->pBuffer[0] = hi2cx->HW_Block->DR;
		     hi2cx->State = READY;
	      }
       if (hi2cx->mode == I2C_RX && hi2cx->RX_Xfer_Size == 2)
              {

		     if (generate_stop (hi2cx) == OK)
		            {
				   hi2cx->pBuffer[0] = hi2cx->HW_Block->DR;

				   hi2cx->pBuffer[1] = hi2cx->HW_Block->DR;
			    }
		     Disable_I2C_Interrrupt (hi2cx);

		     hi2cx->State = READY;

	      }
       /*   else
        {
        if (hi2cx->RX_Xfer_Size - 3 > hi2cx->Xfer_Count)
        {
        hi2cx->pBuffer[hi2cx->Xfer_Count] = hi2cx->HW_Block->DR;
        // hi2cx->HW_Block->CR1 |= (CR1_ACK);
        hi2cx->Xfer_Count++;
        }

        if (CHECK_FLAG(hi2cx, CR2,CR2_ITEVTEN) && CHECK_FLAG(hi2cx, SR1, SR1_BTF))
        {
        if (hi2cx->RX_Xfer_Size - 2 > hi2cx->Xfer_Count)
        {

        hi2cx->HW_Block->CR1 &= ~(CR1_ACK);
        hi2cx->pBuffer[hi2cx->Xfer_Count] = hi2cx->HW_Block->DR;
        hi2cx->Xfer_Count++;
        hi2cx->HW_Block->CR2 &= ~CR2_ITBUFEN;

        }

        if (hi2cx->RX_Xfer_Size - 2 <= hi2cx->Xfer_Count)
        {
        generate_stop (hi2cx);
        hi2cx->pBuffer[hi2cx->Xfer_Count] = hi2cx->HW_Block->DR;
        hi2cx->Xfer_Count++;
        Disable_I2C_Interrrupt (hi2cx);
        hi2cx->pBuffer[hi2cx->Xfer_Count] = hi2cx->HW_Block->DR;
        hi2cx->State = READY;
        hi2cx->pBuffer = NULL;

        }
        }
        }*/

}

void I2C_TX_FINISH (I2C_Handler *hi2cx)
{
       generate_stop (hi2cx);
       Disable_I2C_Interrrupt (hi2cx);
       hi2cx->State = READY;
       hi2cx->pBuffer = NULL;
       hi2cx->TX_Xfer_Size = 0;
       hi2cx->Xfer_Count = 0;
}

void Enable_I2C_Interrrupt (I2C_Handler *hi2cx)
{
       hi2cx->HW_Block->CR2 |= CR2_ITEVTEN | CR2_ITBUFEN | CR2_ITERREN;
}

void Disable_I2C_Interrrupt (I2C_Handler *hi2cx)
{
       hi2cx->HW_Block->CR2 &= ~(CR2_ITEVTEN | CR2_ITBUFEN | CR2_ITERREN);
}

void i2c_enable (I2C_Handler *hi2cx)
{

       hi2cx->HW_Block->CR1 |= CR1_PE;
}

I2C_ERROR_e generate_stop (I2C_Handler *hi2cx)
{
       /*
        * send stop condition??
        */
       hi2cx->HW_Block->CR1 |= CR1_STOP;
       if (hi2cx->HW_Block->CR1 & CR1_STOP)
              {
		     return OK;
	      }
       return noStop;
}
