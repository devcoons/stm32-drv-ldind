/*!
	@file   drv_ldind.c
	@brief  <brief description here>
	@t.odo	-
	---------------------------------------------------------------------------

	MIT License
	Copyright (c) 2022 Daniele Russo, Ioannis Deligiannis, Federico Carnevale

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/******************************************************************************
* Preprocessor Definitions & Macros
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/

#include "drv_ldind.h"


#ifdef DRV_LDIND_ENABLED
/******************************************************************************
 * Enumerations, structures & Variables
 ******************************************************************************/

static ldind_t* ldind_interfaces[8] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
static uint32_t ldind_interfaces_cnt = 0;

static GPIO_TypeDef *ERROR_PORT;
static uint16_t ERROR_PIN;
static uint8_t ACTUAL_STATE;
static TIM_HandleTypeDef * ERROR_HANDLER;

/******************************************************************************
 * Declaration | Static Functions
 ******************************************************************************/

/******************************************************************************
 * Definition  | Static Functions
 ******************************************************************************/

/******************************************************************************
 * Definition  | Public Functions
 ******************************************************************************/

i_status ldind_init(ldind_t* ldind)
{
	ldind->mx_init();

	for(register uint32_t i = 0 ; i < ldind_interfaces_cnt; i++)
	{
		if(ldind_interfaces[i] == ldind)
		{
			return I_OK;
		}
	}

	ldind_interfaces[ldind_interfaces_cnt] = ldind;
	ldind_interfaces_cnt++;

	HAL_GPIO_TogglePin(ldind->gpio_port, ldind->gpio_pin);
	HAL_Delay(100);
	HAL_GPIO_TogglePin(ldind->gpio_port, ldind->gpio_pin);

	return I_OK;
}


i_status ldind_write(ldind_t* h, i_led_condition state)
{
	if(h->mx_init == NULL)
		return I_ERROR;

	switch(state)
	{
	case C_ON:

		if(h->active_high)
		{
			HAL_GPIO_WritePin(h->gpio_port, h->gpio_pin, GPIO_PIN_SET);

		}else{

			HAL_GPIO_WritePin(h->gpio_port, h->gpio_pin, GPIO_PIN_RESET);
		}

		ACTUAL_STATE = C_ON;
		break;

	case C_OFF:

		if(h->active_high)
		{
			HAL_GPIO_WritePin(h->gpio_port, h->gpio_pin, GPIO_PIN_RESET);

		}else{

			HAL_GPIO_WritePin(h->gpio_port, h->gpio_pin, GPIO_PIN_SET);
		}

		ACTUAL_STATE = C_OFF;
		break;

	case C_ERROR:

		ERROR_PORT = h->gpio_port;
		ERROR_PIN = h->gpio_pin;

		ACTUAL_STATE = C_ERROR;
		ERROR_HANDLER = h->htim;

		h->htim->Instance->PSC = h->clock_frequency_mhz - 1;
		h->htim->Instance->CNT = 10000 - 1;

		HAL_TIM_Base_Start_IT(h->htim);
	}

	return I_OK;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == ERROR_HANDLER && ACTUAL_STATE == C_ERROR)
	{
		HAL_GPIO_TogglePin(ERROR_PORT, ERROR_PIN);

	}else{

		HAL_TIM_Base_Stop_IT(htim);
	}
}

/******************************************************************************
 * EOF - NO CODE AFTER THIS LINE
 ******************************************************************************/
#endif
