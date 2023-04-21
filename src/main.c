/********************************************************************************************************//**
* @file main.c
*
* @brief File containing the main function.
**/

#include "logger.h"
#include "usb_driver.h"

/***********************************************************************************************************/
/*                                       Static Function Prototypes                                        */
/***********************************************************************************************************/

/**
 * @brief Function for configuring the GPIO for USB peripheral.
 * @return void.
 */
static inline  __attribute__((always_inline)) void GPIO_Init(void);

/***********************************************************************************************************/
/*                                       Main Function                                                     */
/***********************************************************************************************************/

int main(void){

    log_info("Program entrypoint");

    GPIO_Init();
    USB_Init();
    USB_Connect();

    for(;;);
}

/***********************************************************************************************************/
/*                                       Static Function Definitions                                       */
/***********************************************************************************************************/

static inline  __attribute__((always_inline)) void GPIO_Init(void)
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