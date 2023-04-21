/********************************************************************************************************//**
* @file main.c
*
* @brief File containing the main function.
**/

#include "logger.h"
#include "usb_driver.h"

int main(void){

    log_info("Program entrypoint");

    GPIO_Init();
    USB_Init();
    USB_Connect();

    for(;;);
}