/********************************************************************************************************//**
* @file usb_device.h
*
* @brief Header file containing the typedef and definitions of the USB device.
*
* Public Functions:
*/

#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include "usb_standards.h"
#include <stdint.h>

/**
 * @brief Structure for managing the USB device.
 */
typedef struct
{
    USBDeviceState_t device_state;
    USBControlTransferStage_t control_transfer_stage;
    uint8_t configuration_value;
    void const* ptr_out_buffer;
    uint32_t out_data_size;
    void const* ptr_in_buffer;
    uint32_t in_data_size;
}USB_Device_t;


#endif /* USB_DEVICE_H */