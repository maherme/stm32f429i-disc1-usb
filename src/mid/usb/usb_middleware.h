/********************************************************************************************************//**
* @file usb_middleware.h
*
* @brief Header file containing the prototypes of the APIs for USB middleware.
*
* Public Functions:
*       - void USB_Device_Init(USB_Device_t* usb_device)
*       - void USB_Device_Poll(void)
*/

#ifndef USB_MIDDLEWARE_H
#define USB_MIDDLEWARE_H

#include "usb_driver.h"
#include "usb_device.h"
#include <stdint.h>

/***********************************************************************************************************/
/*                                       APIs Supported                                                    */
/***********************************************************************************************************/

/**
 * @brief Function for initializing the USB peripheral.
 * @param[in] usb_device is a pointer to USB_Device_t struct used for binding static methods to the device.
 * @return void
 */
void USB_Device_Init(USB_Device_t* usb_device);

/**
 * @brief Function for polling the events of the USB.
 * @return void
 */
void USB_Device_Poll(void);

#endif /* USB_MIDDLEWARE_H */