/********************************************************************************************************//**
* @file usb_driver.h
*
* @brief Header file containing the prototypes of the APIs for USB driver.
*
* Public Functions:
*       - void USB_Init(void)
*       - void USB_Connect(void)
*       - void USB_Disconnect(void)
*/

#ifndef USB_DRIVER_H
#define USB_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

#define USB_OTG_HS_GLOBAL ((USB_OTG_GlobalTypeDef*)(USB_OTG_HS_PERIPH_BASE + USB_OTG_GLOBAL_BASE))
#define USB_OTG_HS_DEVICE ((USB_OTG_DeviceTypeDef*)(USB_OTG_HS_PERIPH_BASE + USB_OTG_DEVICE_BASE))
#define USB_OTG_HS_PCGCCTL ((uint32_t*)(USB_OTG_HS_PERIPH_BASE + USB_OTG_PCGCCTL_BASE))

/***********************************************************************************************************/
/*                                       APIs Supported                                                    */
/***********************************************************************************************************/

/**
 * @brief Function for initializing the USB peripheral.
 * @return void.
 */
void USB_Init(void);

/**
 * @brief Function for connecting the USB peripheral to the bus.
 * @return void.
 */
void USB_Connect(void);

/**
 * @brief Function for disconnecting the USB peripheral to the bus.
 * @return void.
 */
void USB_Disconnect(void);

#endif /* USB_DRIVER_H */