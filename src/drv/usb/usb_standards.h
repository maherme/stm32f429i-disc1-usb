/********************************************************************************************************//**
* @file usb_standards.h
*
* @brief Header file containing the typedef and definitions of the USB standard.
*
* Public Functions:
*/

#ifndef USB_STANDARDS_H
#define USB_STANDARDS_H

#include <stdint.h>

/**
 * @name USB device bit mapped request type fields.
 * @{
 */
#define USB_BM_REQUEST_TYPE_DIRECTION_MASK      (1 << 7)
#define USB_BM_REQUEST_TYPE_DIRECTION_TODEVICE  (0 << 7)
#define USB_BM_REQUEST_TYPE_DIRECTION_TOHOST    (1 << 7)
#define USB_BM_REQUEST_TYPE_TYPE_MASK           (3 << 5)
#define USB_BM_REQUEST_TYPE_TYPE_STANDARD       (0 << 5)
#define USB_BM_REQUEST_TYPE_TYPE_CLASS          (1 << 5)
#define USB_BM_REQUEST_TYPE_TYPE_VENDOR         (2 << 5)
#define USB_BM_REQUEST_TYPE_RECIPIENT_MASK      (3 << 0)
#define USB_BM_REQUEST_TYPE_RECIPIENT_DEVICE    (0 << 0)
#define USB_BM_REQUEST_TYPE_RECIPIENT_INTERFACE (1 << 0)
#define USB_BM_REQUEST_TYPE_RECIPIENT_ENDPOINT  (2 << 0)
#define USB_BM_REQUEST_TYPE_RECIPIENT_OTHER     (3 << 0)
/** @} */

/**
 * @name USB standard requests.
 * @{
 */
#define USB_STANDARD_GET_STATUS         0x00
#define USB_STANDARD_CLEAR_FEATURE      0x01
#define USB_STANDARD_SET_FEATURE        0x03
#define USB_STANDARD_SET_ADDRESS        0x05
#define USB_STANDARD_GET_DESCRIPTOR     0x06
#define USB_STANDARD_SET_DESCRIPTOR     0x07
#define USB_STANDARD_GET_CONFIG         0x08
#define USB_STANDARD_SET_CONFIG         0x09
#define USB_STANDARD_GET_INTERFACE      0x0A
#define USB_STANDARD_SET_INTERFACE      0x0B
#define USB_STANDARD_SYNCH_FRAME        0x0C
/** @} */

/**
 * @name USB standard descriptor types.
 * @{
 */
#define USB_DESCRIPTOR_TYPE_DEVICE          0x01
#define USB_DESCRIPTOR_TYPE_CONFIGURATION   0x02
#define USB_DESCRIPTOR_TYPE_STRING          0x03
#define USB_DESCRIPTOR_TYPE_INTERFACE       0x04
#define USB_DESCRIPTOR_TYPE_ENDPOINT        0x05
#define USB_DESCRIPTOR_TYPE_QUALIFIER       0x06
#define USB_DESCRIPTOR_TYPE_OTHER           0x07
#define USB_DESCRIPTOR_TYPE_INTERFACEPOWER  0x08
#define USB_DESCRIPTOR_TYPE_OTG             0x09
#define USB_DESCRIPTOR_TYPE_DEBUG           0x0A
#define USB_DESCRIPTOR_TYPE_INTERFACEASSOC  0x0B
#define USB_DESCRIPTOR_TYPE_CS_INTERFACE    0x24
#define USB_DESCRIPTOR_TYPE_CS_ENDPOINT     0x25
/** @} */

/**
 * @name USB classes.
 * @{
 */
#define USB_CLASS_PER_INTERFACE 0x00
#define USB_CLASS_AUDIO         0x01
#define USB_CLASS_PHYSICAL      0x05
#define USB_CLASS_STILL_IMAGE   0x06
#define USB_CLASS_PRINTER       0x07
#define USB_CLASS_MASS_STORAGE  0x08
#define USB_CLASS_HUB           0x09
#define USB_CLASS_CSCID         0x0B
#define USB_CLASS_CONTENT_SEC   0x0D
#define USB_CLASS_VIDEO         0x0E
#define USB_CLASS_HEALTHCARE    0x0F
#define USB_CLASS_AV            0x10
#define USB_CLASS_BILLBOARD     0x11
#define USB_CLASS_CBRIDGE       0x12
#define USB_CLASS_DIAGNOSTIC    0xDC
#define USB_CLASS_WIRELESS      0xE0
#define USB_CLASS_MISC          0xEF
#define USB_CLASS_IAD           0xEF
#define USB_CLASS_APP_SPEC      0xFE
#define USB_CLASS_VENDOR        0xFF

#define USB_SUBCLASS_NONE       0x00
#define USB_SUBCLASS_IAD        0x02
#define USB_SUBCLASS_VENDOR     0xFF

#define USB_PROTOCOL_NONE       0x00
#define USB_PROTOCOL_IAD        0x01
#define USB_PROTOCOL_VENDOR     0xFF
/** @} */

/**
 * @brief List of endpoint types.
 */
typedef enum
{
    USB_ENDPOINT_TYPE_CONTROL,
    USB_ENDPOINT_TYPE_ISOCHRONOUS,
    USB_ENDPOINT_TYPE_BULK,
    USB_ENDPOINT_TYPE_INTERRUPT
}USBEndpointType_t;

/**
 * @brief List of possible states of the USB device.
 */
typedef enum
{
    USB_DEVICE_STATE_DEFAULT,
    USB_DEVICE_STATE_ADDRESSED,
    USB_DEVICE_STATE_CONFIGURED,
    USB_DEVICE_STATE_SUSPENDED
}USBDeviceState_t;

/**
 * @brief List of possible states of the control transfer of the USB.
 */
typedef enum
{
    USB_CONTROL_STAGE_SETUP,
    USB_CONTROL_STAGE_DATA_OUT,
    USB_CONTROL_STAGE_DATA_IN,
    USB_CONTROL_STAGE_DATA_IN_IDLE,
    USB_CONTROL_STAGE_DATA_IN_ZERO,
    USB_CONTROL_STAGE_STATUS_OUT,
    USB_CONTROL_STAGE_STATUS_IN
}USBControlTransferStage_t;

/**
 * @brief Struct for storing pointers to functions for managing the USB events.
 */
typedef struct
{
    void(*USB_Reset_Received)(void);
    void(*USB_Setup_Data_Received)(uint8_t endpoint_number, uint16_t byte_cnt);
    void(*USB_Out_Data_Received)(uint8_t endpoint_number, uint16_t bcnt);
    void(*USB_In_Transfer_Completed)(uint8_t endpoint_number);
    void(*USB_Out_Transfer_Completed)(uint8_t endpoint_number);
    void(*USB_Polled)(void);
}USB_Events_t;

/**
 * @brief Struct for accesing to the setup packet fields.
 */
typedef struct
{
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
}USB_Request_t;

/**
 * @brief Struct which the USB standard device dercriptor fields.
 */
typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
}USB_StdDeviceDescriptor_t;

#endif /* USB_STANDARDS_H */