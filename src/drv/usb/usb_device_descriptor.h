/********************************************************************************************************//**
* @file usb_device_descriptor.h
*
* @brief Header file containing the typedef and definitions of the USB device descriptor.
*
* Public Functions:
*/

#ifndef USB_DEVICE_DESCRIPTOR_H
#define USB_DEVICE_DESCRIPTOR_H

#include "usb_standards.h"

const USB_StdDeviceDescriptor_t device_descriptor = {
    .bLength = sizeof(USB_StdDeviceDescriptor_t),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = USB_CLASS_PER_INTERFACE,
    .bDeviceSubClass = USB_SUBCLASS_NONE,
    .bDeviceProtocol = USB_PROTOCOL_NONE,
    .idVendor = 0x6666,
    .idProduct = 0x13AA,
    .bcdDevice = 0x0100,
    .iManufacturer = 0,
    .iProduct = 0,
    .iSerialNumber = 0,
    .bNumConfigurations = 1
};

typedef struct
{
    USB_StdCfgDescriptor_t usb_configuration_descriptor;
}USB_CfgDescriptorCombination_t;

const USB_CfgDescriptorCombination_t cfg_descriptor_combination = {
    .usb_configuration_descriptor = {

    }
};

#endif /* USB_DEVICE_DESCRIPTOR_H */