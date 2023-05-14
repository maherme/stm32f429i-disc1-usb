/************************************************************************************************//**
* @file usb_hid_standards.h
*
* @brief Header file containing the typedef and definitions of the USB HID standard.
*/

#ifndef USB_HID_STANDARDS_H
#define USB_HID_STANDARDS_H

#include <stdint.h>

#define USB_DESCRIPTOR_TYPE_HID         0x21
#define USB_DESCRIPTOR_TYPE_HID_REPORT  0x22
#define USB_HID_COUNTRY_NONE            0

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;
    uint8_t bDescriptorType0;
    uint16_t wDescriptorLength0;
} __attribute__((__packed__)) USB_HIDDescriptor_t;

#endif /* USB_HID_STANDARDS */