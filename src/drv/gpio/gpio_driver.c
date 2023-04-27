/********************************************************************************************************//**
* @file gpio_driver.c
*
* @brief File containing the APIs for the GPIO peripheral.
*
* Public Functions:
*       - void GPIO_Init(void)
*
* @note
*       For further information about functions refer to the corresponding header file.
**/

#include "gpio_driver.h"
#include "stm32f4xx.h"
#include <stdint.h>

void GPIO_Init(void)
{
    /* Enable GPIOB Clock */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);

    /* Set alternate function for PB14 (-) and PB15 (+) */
    MODIFY_REG(
        GPIOB->AFR[1],
        GPIO_AFRH_AFSEL14 | GPIO_AFRH_AFSEL15,
        _VAL2FLD(GPIO_AFRH_AFSEL14, 0xC) | _VAL2FLD(GPIO_AFRH_AFSEL15, 0xC)
    );

    /* Configure USB pins to work in alternate function mode */
    MODIFY_REG(
        GPIOB->MODER,
        GPIO_MODER_MODER14 | GPIO_MODER_MODER15,
        _VAL2FLD(GPIO_MODER_MODER14, 2) | _VAL2FLD(GPIO_MODER_MODER15, 2)
    );
}