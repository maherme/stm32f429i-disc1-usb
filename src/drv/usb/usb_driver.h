/********************************************************************************************************//**
* @file usb_driver.h
*
* @brief Header file containing the prototypes of the APIs for USB driver.
*
* Public Functions:
*/

#ifndef USB_DRIVER_H
#define USB_DRIVER_H

#include "stm32f4xx.h"
#include "usb_standards.h"
#include <stdint.h>

#define USB_OTG_HS_GLOBAL ((USB_OTG_GlobalTypeDef*)(USB_OTG_HS_PERIPH_BASE + USB_OTG_GLOBAL_BASE))
#define USB_OTG_HS_DEVICE ((USB_OTG_DeviceTypeDef*)(USB_OTG_HS_PERIPH_BASE + USB_OTG_DEVICE_BASE))
#define USB_OTG_HS_PCGCCTL ((uint32_t*)(USB_OTG_HS_PERIPH_BASE + USB_OTG_PCGCCTL_BASE))

/* @brief Number of IN or OUT endpoints */
#define USB_ENDPOINT_COUNT      6

/**
 * @brief Structure for managing public APIs of USB driver.
 */
typedef struct
{
    void(*USB_Init)(void);
    void(*USB_Set_Device_Address)(uint8_t address);
    void(*USB_Connect)(void);
    void(*USB_Disconnect)(void);
    void(*USB_Flush_RxFIFO)(void);
    void(*USB_Flush_TxFIFO)(uint8_t endpoint_number);
    void(*USB_Configure_IN_Endpoint)(uint8_t endpoint_number,
                                    USBEndpointType_t endpoint_type,
                                    uint16_t endpoint_size);
    void(*USB_Read_Packet)(const void* buffer, uint16_t size);
    void(*USB_Write_Packet)(uint8_t endpoint_number, void const* buffer, uint16_t size);
    void(*USB_Poll)(void);
}USB_Driver_t;

/***********************************************************************************************************/
/*                                       Exported Variables                                                */
/***********************************************************************************************************/

extern const USB_Driver_t USB_driver;
extern USB_Events_t USB_events;

/***********************************************************************************************************/
/*                                       APIs Supported                                                    */
/***********************************************************************************************************/

#endif /* USB_DRIVER_H */