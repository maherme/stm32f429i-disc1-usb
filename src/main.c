/********************************************************************************************************//**
* @file main.c
*
* @brief File containing the main function.
**/

#include "logger.h"
#include "gpio_driver.h"
#include "usb_middleware.h"
#include <stdint.h>

/***********************************************************************************************************/
/*                                       Static Variables                                                  */
/***********************************************************************************************************/

USB_Device_t usb_device;
uint32_t buffer[8];

/***********************************************************************************************************/
/*                                       Main Function                                                     */
/***********************************************************************************************************/

int main(void){

    log_info("Program entrypoint");

    usb_device.ptr_out_buffer = &buffer;

    GPIO_Init();
    USB_Device_Init(&usb_device);

    for(;;){
        USB_Device_Poll();
    }
}