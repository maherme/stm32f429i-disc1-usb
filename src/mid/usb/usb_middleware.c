/********************************************************************************************************//**
* @file usb_middleware.c
*
* @brief File containing the APIs for USB middleware.
*
* Public Functions:
*       - void USB_Device_Init(USB_Device_t* usb_device)
*       - void USB_Device_Poll(void)
*
* @note
*       For further information about functions refer to the corresponding header file.
**/

#include "usb_middleware.h"
#include "usb_driver.h"
#include "usb_device_descriptor.h"
#include "usb_standards.h"
#include "logger.h"
#include <stdint.h>

/***********************************************************************************************************/
/*                                       Static Variables                                                  */
/***********************************************************************************************************/

static USB_Device_t* usb_device_handle;

/***********************************************************************************************************/
/*                                       Static Function Prototypes                                        */
/***********************************************************************************************************/

/**
 * @brief Function for managing a received reset event.
 * @return void
 */
static void USB_Reset_Received_Handler(void);

/**
 * @brief Function for managing a received setup data event.
 * @param[in] endpoint_number is the endpoint number from the event is received.
 * @param[in] byte_cnt is the amount of received bytes.
 * @return void
 */
static void USB_Setup_Data_Received_Handler(uint8_t endpoint_number, uint16_t byte_cnt);

static void USB_Polled_Handler(void);

/**
 * @brief Function for processing a received request.
 * @return void
 */
static void process_request(void);

static void process_standard_device_request(const USB_Request_t* request);

static void process_control_transfer_stage(void);

/***********************************************************************************************************/
/*                                       Global Variables                                                  */
/***********************************************************************************************************/

USB_Events_t USB_events = {
    .USB_Reset_Received = &USB_Reset_Received_Handler,
    .USB_Setup_Data_Received = &USB_Setup_Data_Received_Handler,
    .USB_Polled = &USB_Polled_Handler
};

/***********************************************************************************************************/
/*                                       Public API Definitions                                            */
/***********************************************************************************************************/

void USB_Device_Init(USB_Device_t* usb_device)
{
    usb_device_handle = usb_device;
    USB_driver.USB_Init();
    USB_driver.USB_Connect();
}

void USB_Device_Poll(void)
{
    USB_driver.USB_Poll();
}

/***********************************************************************************************************/
/*                                       Static Function Definitions                                       */
/***********************************************************************************************************/

static void USB_Reset_Received_Handler(void)
{
    usb_device_handle->in_data_size = 0;
    usb_device_handle->out_data_size = 0;
    usb_device_handle->configuration_value = 0;
    usb_device_handle->device_state = USB_DEVICE_STATE_DEFAULT;
    usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_SETUP;
    USB_driver.USB_Set_Device_Address(0);
}

static void USB_Setup_Data_Received_Handler(uint8_t endpoint_number, uint16_t byte_cnt)
{
    USB_driver.USB_Read_Packet(usb_device_handle->ptr_out_buffer, byte_cnt);
    log_debug_array("SETUP data: ", usb_device_handle->ptr_out_buffer, byte_cnt);
    process_request();
}

static void USB_Polled_Handler(void)
{
    process_control_transfer_stage();
}

static void process_request(void)
{
    const USB_Request_t* request = usb_device_handle->ptr_out_buffer;

    switch(request->bmRequestType & (USB_BM_REQUEST_TYPE_TYPE_MASK | USB_BM_REQUEST_TYPE_RECIPIENT_MASK)){
        case USB_BM_REQUEST_TYPE_TYPE_STANDARD | USB_BM_REQUEST_TYPE_RECIPIENT_DEVICE:
            break;
        default:
            /* do nothing */
            break;
    }
}

static void process_standard_device_request(const USB_Request_t* request)
{
    const uint8_t descriptor_type = request->wValue >> 8;
    const uint16_t descriptor_length = request->wLength;

    switch(request->bRequest){
        case USB_STANDARD_GET_DESCRIPTOR:
            log_info("Standard Get Descriptor request received");
            switch(descriptor_type){
                case USB_DESCRIPTOR_TYPE_DEVICE:
                    log_info("- Get Device Descriptor");
                    usb_device_handle->ptr_in_buffer = &device_descriptor;
                    usb_device_handle->in_data_size = descriptor_length;
                    log_info("Switching control stage to IN-DATA");
                    usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN;
                    break;
                default:
                    /* do nothing */
                    break;
            }
            break;
        default:
            /* do nothing */
            break;
    }
}

static void process_control_transfer_stage(void)
{
    uint8_t data_size = device_descriptor.bMaxPacketSize0;

    switch(usb_device_handle->control_transfer_stage){
        case USB_CONTROL_STAGE_SETUP:
            /* do nothing */
            break;
        case USB_CONTROL_STAGE_DATA_IN:
            log_info("Processing IN-DATA stage");
            USB_driver.USB_Write_Packet(0, usb_device_handle->ptr_in_buffer, data_size);
            usb_device_handle->in_data_size -= data_size;
            usb_device_handle->ptr_in_buffer += data_size;
            usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN_IDLE;
            break;
        case USB_CONTROL_STAGE_DATA_IN_IDLE:
            /* do nothing */
            break;
        default:
            /* do nothing */
            break;
    }
}