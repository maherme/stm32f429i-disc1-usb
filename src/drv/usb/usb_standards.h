/************************************************************************************************//**
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
 * @defgroup USB_BM_REQUEST_TYPE USB Device Bit Mapped Request Type Fields.
 * @brief Bit mapped request type fields for managing the @ref USB_STD_REQUESTS
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
 * @defgroup USB_STD_REQUESTS USB Standard Requests.
 * @brief USB standard requests which a host can send.
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
 * @defgroup USB_STD_DESCRIPTOR_TYPES USB Standard Descriptor Types.
 * @brief Used for indicating the type of descriptor.
 * @{
 */
/** @brief Device */
#define USB_DESCRIPTOR_TYPE_DEVICE          0x01
/** @brief Configuration */
#define USB_DESCRIPTOR_TYPE_CONFIGURATION   0x02
/** @brief String (Optional) */
#define USB_DESCRIPTOR_TYPE_STRING          0x03
/** @brief Interface */
#define USB_DESCRIPTOR_TYPE_INTERFACE       0x04
/** @brief Endpoint */
#define USB_DESCRIPTOR_TYPE_ENDPOINT        0x05
/** @brief Device qualifier (only for devices that supports both full and high speeds) */
#define USB_DESCRIPTOR_TYPE_QUALIFIER       0x06
/** @brief Other speed configuration (only for devices that supports both full and high speeds) */
#define USB_DESCRIPTOR_TYPE_OTHER           0x07
/** @brief Interfae power */
#define USB_DESCRIPTOR_TYPE_INTERFACEPOWER  0x08
/** @brief Only for On-The-Go devices */
#define USB_DESCRIPTOR_TYPE_OTG             0x09
/** @brief Debug */
#define USB_DESCRIPTOR_TYPE_DEBUG           0x0A
/** @brief Interface association (Only for composite devices) */
#define USB_DESCRIPTOR_TYPE_INTERFACEASSOC  0x0B
#define USB_DESCRIPTOR_TYPE_CS_INTERFACE    0x24
#define USB_DESCRIPTOR_TYPE_CS_ENDPOINT     0x25
/** @} */

/**
 * @defgroup USB_STD_CLASSES USB Class Codes.
 * @brief USB defines class code information that is used to identify a device’s functionality and to
 *        nominally load a device driver based on that functionality.
 * @{
 */
#define USB_CLASS_PER_INTERFACE 0x00
#define USB_CLASS_AUDIO         0x01
#define USB_CLASS_HID           0x03
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
 * @brief List of possible states for the control transfer of the USB.
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
    /** @brief The direction of the request, type of request and designated recipient */
    uint8_t bmRequestType;
    /** @brief Request identity */
    uint8_t bRequest;
    /** @brief Parameter passed with the request */
    uint16_t wValue;
    /** @brief Parameter passed with the request */
    uint16_t wIndex;
    /** @brief Number of bytes to transfer if there is a data phase */
    uint16_t wLength;
}USB_Request_t;

/**
 * @brief Struct with the USB standard device dercriptor fields.
 */
typedef struct
{
    /** @brief Provides the length of the descriptor in bytes */
    uint8_t bLength;
    /** @brief Must be value of @ref USB_DESCRIPTOR_TYPE_DEVICE */
    uint8_t bDescriptorType; 
    /** @brief USB specification release number */
    uint16_t bcdUSB;
    /** @brief USB device class */
    uint8_t bDeviceClass;
    /** @brief USB device subclass */
    uint8_t bDeviceSubClass;
    /** @brief USB device protocol */
    uint8_t bDeviceProtocol;
    /** @brief Maximum packet size for endpoint 0 (8, 16, 32 or 64 bytes) */
    uint8_t bMaxPacketSize0;
    /** @brief Vendor ID for the USB device */
    uint16_t idVendor;
    /** @brief Product ID for the USB device */
    uint16_t idProduct;
    /** @brief Device release number */
    uint16_t bcdDevice;
    /** @brief String descriptor index for the manufacturer's name */
    uint8_t iManufacturer;
    /** @brief String descriptor index for the product name */
    uint8_t iProduct;
    /** @brief String descriptor index for the product serial number */
    uint8_t iSerialNumber;
    /** @brief Total number of supported configurations by the USB device */
    uint8_t bNumConfigurations;
} __attribute__((__packed__)) USB_StdDeviceDescriptor_t;

/**
 * @brief Struct with the USB configuration dercriptor fields.
 */
typedef struct
{
    /** @brief Provides the length of the descriptor in bytes */
    uint8_t bLength;
    /** @brief Must be value of @ref USB_DESCRIPTOR_TYPE_CONFIGURATION */
    uint8_t bDescriptorType;
    /** @brief The number of bytes in the configuration descriptor and its subordinate descriptors */
    uint16_t wTotalLength;
    /** @brief Number of interfaces in the configuration */
    uint8_t bNumInterfaces;
    /** @brief Value that identifies the selected configuration */
    uint8_t bConfigurationValue;
    /** @brief Index of string descriptor for configuration details */
    uint8_t iConfiguration;
    /** @brief Self/Bus powered and wakeup settings */
    uint8_t bmAttributes;
    /** @brief Device current requirement */
    uint8_t bMaxPower;
} __attribute__((__packed__)) USB_StdCfgDescriptor_t;

/**
 * @brief Struct with the USB interface dercriptor fields.
 */
typedef struct
{
    /** @brief Provides the length of the descriptor in bytes */
    uint8_t bLength;
    /** @brief Must be value of @ref USB_DESCRIPTOR_TYPE_INTERFACE */
    uint8_t bDescriptorType;
    /** @brief Identify number for this interface */
    uint8_t bInterfaceNumber;
    /** @brief Selected alternate setting for interface */
    uint8_t bAlternateSetting;
    /** @brief Number of endpoints supported by interface */
    uint8_t bNumEndpoints;
    /** @brief Interface call ID */
    uint8_t bInterfaceClass;
    /** @brief Interface subclass ID */
    uint8_t bInterfaceSubClass;
    /** @brief Interface protocol ID */
    uint8_t bInterfaceProtocol;
    /** @brief Index of the string descriptor describing the interface */
    uint8_t iInterface;
} __attribute__((__packed__)) USB_InterfaceDescriptor_t;

/**
 * @brief Struct with the USB endpoint dercriptor fields.
 */
typedef struct
{
    /** @brief Provides the length of the descriptor in bytes */
    uint8_t bLength;
    /** @brief Must be value of @ref USB_DESCRIPTOR_TYPE_ENDPOINT */
    uint8_t bDescriptorType;
    /** @brief Logical address of the endpoint including direction mask */
    uint8_t bEndpointAddress;
    /** @brief Endpoint attributes, possible value from @ref USBEndpointType_t */
    uint8_t bmAttributes;
    /** @brief Maximum packet size of the endpoint */
    uint16_t wMaxPacketSize;
    /** @brief Polling interval of the endpoint (frames) */
    uint8_t bInterval;
} __attribute__((__packed__)) USB_EndpointDescriptor_t;

#endif /* USB_STANDARDS_H */