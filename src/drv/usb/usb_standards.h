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

#endif /* USB_STANDARDS_H */