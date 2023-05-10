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
#include "usb_hid_standards.h"
#include "usb_hid.h"
#include "hid_usage_desktop.h"
#include "hid_usage_button.h"
#include <stdint.h>

const USB_StdDeviceDescriptor_t device_descriptor = {
    .bLength = sizeof(USB_StdDeviceDescriptor_t),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = USB_CLASS_PER_INTERFACE,
    .bDeviceSubClass = USB_SUBCLASS_NONE,
    .bDeviceProtocol = USB_PROTOCOL_NONE,
    .bMaxPacketSize0 = 8,
    .idVendor = 0x6666,
    .idProduct = 0x13AA,
    .bcdDevice = 0x0100,
    .iManufacturer = 0,
    .iProduct = 0,
    .iSerialNumber = 0,
    .bNumConfigurations = 1
};

const uint8_t hid_report_descriptor[] = {
    HID_USAGE_PAGE(HID_PAGE_DESKTOP),
    HID_USAGE(HID_DESKTOP_MOUSE),
    HID_COLLECTION(HID_APPLICATION_COLLECTION),
        HID_USAGE(HID_DESKTOP_POINTER),
        HID_COLLECTION(HID_PHYSICAL_COLLECTION),
            HID_USAGE(HID_DESKTOP_X),
            HID_USAGE(HID_DESKTOP_Y),
            HID_LOGICAL_MINIMUM(-127),
            HID_LOGICAL_MAXIMUM(127),
            HID_REPORT_SIZE(8),
            HID_REPORT_COUNT(2),
            HID_INPUT(HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

            HID_USAGE_PAGE(HID_PAGE_BUTTON),
            HID_USAGE_MINIMUM(1),
            HID_USAGE_MAXIMUM(3),
            HID_LOGICAL_MINIMUM(0),
            HID_LOGICAL_MAXIMUM(1),
            HID_REPORT_SIZE(1),
            HID_REPORT_COUNT(3),
            HID_INPUT(HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
            HID_REPORT_SIZE(1),             /* Padding */
            HID_REPORT_COUNT(5),            /* Padding */
            HID_INPUT(HID_IOF_CONSTANT),    /* Padding */
        HID_END_COLLECTION,
    HID_END_COLLECTION
};

typedef struct
{
    USB_StdCfgDescriptor_t usb_configuration_descriptor;
    USB_InterfaceDescriptor_t usb_interface_descriptor;
    USB_HIDDescriptor_t usb_mouse_hid_descriptor;
    USB_EndpointDescriptor_t usb_mouse_endpoint_descriptor;
}USB_CfgDescriptorCombination_t;

const USB_CfgDescriptorCombination_t cfg_descriptor_combination = {
    .usb_configuration_descriptor = {
        .bLength = sizeof(USB_StdCfgDescriptor_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION,
        .wTotalLength = sizeof(USB_CfgDescriptorCombination_t),
        .bNumInterfaces = 1,
        .bConfigurationValue = 1,
        .iConfiguration = 0,
        .bmAttributes = 0x80 | 0x40,
        .bMaxPower = 25     /* The device may need 50 mW */
    },
    .usb_interface_descriptor = {
        .bLength = sizeof(USB_InterfaceDescriptor_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
        .bInterfaceNumber = 1,
        .bAlternateSetting = 0,
        .bNumEndpoints = 1,
        .bInterfaceClass = USB_CLASS_HID,
        .bInterfaceSubClass = USB_SUBCLASS_NONE,
        .bInterfaceProtocol = USB_PROTOCOL_NONE,
        .iInterface = 0
    },
    .usb_mouse_endpoint_descriptor = {
        .bLength = sizeof(USB_EndpointDescriptor_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
        .bEndpointAddress = 0x83,   /* MSB is set due to 0x03 address is for in endpoint */
        .bmAttributes = USB_ENDPOINT_TYPE_INTERRUPT,
        .wMaxPacketSize = 64,
        .bInterval = 50     /* Units for the interval are frames */
    },
    .usb_mouse_hid_descriptor = {
        .bLength = sizeof(USB_HIDDescriptor_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_HID,
        .bcdHID = 0x0100,
        .bCountryCode = USB_HID_COUNTRY_NONE,
        .bNumDescriptors = 1,
        .bDescriptorType0 = USB_DESCRIPTOR_TYPE_HID_REPORT,
        .wDescriptorLength0 = sizeof(hid_report_descriptor)
    }
};

typedef struct
{
    int8_t x;
    int8_t y;
    uint8_t buttons;
} __attribute__((__packed__)) HID_Report_t;

#endif /* USB_DEVICE_DESCRIPTOR_H */