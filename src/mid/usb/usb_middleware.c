/************************************************************************************************//**
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
#include "usb_hid.h"
#include "logger.h"
#include "helper_math.h"
#include <stdint.h>
#include <stddef.h>

/***************************************************************************************************/
/*                                       Static Variables                                          */
/***************************************************************************************************/

static USB_Device_t* usb_device_handle;

/***************************************************************************************************/
/*                                       Static Function Prototypes                                */
/***************************************************************************************************/

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
static void USB_Setup_Data_Received_Handler(
    __attribute__((unused)) uint8_t endpoint_number,
    uint16_t byte_cnt
);

/**
 * @brief Function for managing the poll event.
 * @return void
*/
static void USB_Polled_Handler(void);

/**
 * @brief Function for completing an IN transfer.
 * @param[in] endpoint_number is the endpoint number for the IN transfer.
 * @return void
 */
static void USB_In_Transfer_Completed_Handler(uint8_t endpoint_number);

/**
 * @brief Function for completing an OUT transfer.
 * @param[in] endpoint_number is the endpoint number for the OUT transfer.
 * @return void
 */
static void USB_Out_Transfer_Completed_Handler(__attribute__((unused)) uint8_t endpoint_number);

/**
 * @brief Function for setting the configuration of the device.
 * @return void
*/
static void USB_Device_Configure(void);

/**
 * @brief Function for processing a received request.
 * @return void
 */
static void process_request(void);

/**
 * @brief Function for processing an standard USB request.
 * @param[in] request is a pointer to the received request.
 * @return void
 */
static void process_standard_device_request(const USB_Request_t* request);

/**
 * @brief Function for processing an interface USB request.
 * @param[in] request is a pointer to the received request.
 * @return void
 */
static void process_class_interface_request(const USB_Request_t* request);

/**
 * @brief Function for processing an standard interface USB request.
 * @param[in] request is a pointer to the received request.
 * @return void
 */
static void process_standard_interface_request(const USB_Request_t* request);

/**
 * @brief Function implementing the finite state machine for controlling the transfer stages of the 
 *        USB device.
 * @return void
 */
static void process_control_transfer_stage(void);

/**
 * @brief Function for sendind the report of the mouse, it will send information for moving the
 *        mouse to the rigth.
 * @return void
*/
static void write_mouse_report(void);

/***************************************************************************************************/
/*                                       Global Variables                                          */
/***************************************************************************************************/

USB_Events_t USB_events = {
    .USB_Reset_Received = &USB_Reset_Received_Handler,
    .USB_Setup_Data_Received = &USB_Setup_Data_Received_Handler,
    .USB_Polled = &USB_Polled_Handler,
    .USB_In_Transfer_Completed = &USB_In_Transfer_Completed_Handler,
    .USB_Out_Transfer_Completed = &USB_Out_Transfer_Completed_Handler
};

/***************************************************************************************************/
/*                                       Public API Definitions                                    */
/***************************************************************************************************/

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

/***************************************************************************************************/
/*                                       Static Function Definitions                               */
/***************************************************************************************************/

static void USB_Reset_Received_Handler(void)
{
    usb_device_handle->in_data_size = 0;
    usb_device_handle->out_data_size = 0;
    usb_device_handle->configuration_value = 0;
    usb_device_handle->device_state = USB_DEVICE_STATE_DEFAULT;
    usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_SETUP;
    USB_driver.USB_Set_Device_Address(0);
}

static void USB_Setup_Data_Received_Handler(
    __attribute__((unused))uint8_t endpoint_number,
    uint16_t byte_cnt)
{
    USB_driver.USB_Read_Packet(usb_device_handle->ptr_out_buffer, byte_cnt);
    log_debug_array("SETUP data: ", usb_device_handle->ptr_out_buffer, byte_cnt);
    process_request();
}

static void USB_Polled_Handler(void)
{
    process_control_transfer_stage();
}

static void USB_In_Transfer_Completed_Handler(uint8_t endpoint_number)
{
    if(usb_device_handle->in_data_size){
        log_info("Switching control stage to IN-DATA");
        usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN;
    }
    else if(usb_device_handle->control_transfer_stage == USB_CONTROL_STAGE_DATA_IN_ZERO){
        USB_driver.USB_Write_Packet(0, NULL, 0);
        log_info("Switching control stage to OUT STATUS");
        usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_STATUS_OUT;
    }

    if(endpoint_number ==
       (cfg_descriptor_combination.usb_mouse_endpoint_descriptor.bEndpointAddress & 0x0F)){
        write_mouse_report();
    }
}

static void USB_Out_Transfer_Completed_Handler(__attribute__((unused)) uint8_t endpoint_number)
{
    /* To be defined */
}

static void USB_Device_Configure(void)
{
    USB_driver.USB_Configure_IN_Endpoint(
        (cfg_descriptor_combination.usb_mouse_endpoint_descriptor.bEndpointAddress & 0x0F),
        (cfg_descriptor_combination.usb_mouse_endpoint_descriptor.bmAttributes & 0x03),
        cfg_descriptor_combination.usb_mouse_endpoint_descriptor.wMaxPacketSize
    );

    /* For confirming the configuration send a status IN packet (view reference manual) */
    USB_driver.USB_Write_Packet(
        (cfg_descriptor_combination.usb_mouse_endpoint_descriptor.bEndpointAddress & 0x0F),
        NULL,
        0
    );
}

static void process_request(void)
{
    const USB_Request_t* request = usb_device_handle->ptr_out_buffer;

    switch(request->bmRequestType & 
           (USB_BM_REQUEST_TYPE_TYPE_MASK | USB_BM_REQUEST_TYPE_RECIPIENT_MASK)){
        case USB_BM_REQUEST_TYPE_TYPE_STANDARD | USB_BM_REQUEST_TYPE_RECIPIENT_DEVICE:
            process_standard_device_request(request);
            break;
        case USB_BM_REQUEST_TYPE_TYPE_CLASS | USB_BM_REQUEST_TYPE_RECIPIENT_INTERFACE:
            process_class_interface_request(request);
            break;
        case USB_BM_REQUEST_TYPE_TYPE_STANDARD | USB_BM_REQUEST_TYPE_RECIPIENT_INTERFACE:
            process_standard_interface_request(request);
            break;
        default:
            /* do nothing */
            break;
    }
}

static void process_standard_device_request(const USB_Request_t* request)
{
    uint8_t descriptor_type = 0;
    uint16_t descriptor_length = 0;
    uint16_t device_address = 0;
//    uint8_t descriptor_index = 0;     /* Can be needed for getting the descriptor index */

    switch(request->bRequest){
        case USB_STANDARD_GET_DESCRIPTOR:
            log_info("Standard Get Descriptor request received");
            descriptor_type = request->wValue >> 8;
            descriptor_length = request->wLength;
            /* Can be needed for getting the descriptor index */
//            descriptor_index = request->wValue & 0xFF;
            switch(descriptor_type){
                case USB_DESCRIPTOR_TYPE_DEVICE:
                    log_info("- Get Device Descriptor");
                    usb_device_handle->ptr_in_buffer = &device_descriptor;
                    usb_device_handle->in_data_size = descriptor_length;
                    log_info("Switching control transfer stage to IN-DATA");
                    usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN;
                    break;
                case USB_DESCRIPTOR_TYPE_CONFIGURATION:
                    log_info("- Get Configuration Descriptor");
                    usb_device_handle->ptr_in_buffer = &cfg_descriptor_combination;
                    usb_device_handle->in_data_size = descriptor_length;
                    log_info("Switching control transfer stage to IN-DATA");
                    usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN;
                    break;
                default:
                    /* do nothing */
                    break;
            }
            break;
        case USB_STANDARD_SET_ADDRESS:
            log_info("Standard Set Address request received");
            device_address = request->wValue;
            USB_driver.USB_Set_Device_Address(device_address);
            usb_device_handle->device_state = USB_DEVICE_STATE_ADDRESSED;
            log_info("Switching control transfer stage to IN-STATUS");
            usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_STATUS_IN;
            break;
        case USB_STANDARD_SET_CONFIG:
            log_info("Standard Set Configuration request received");
            usb_device_handle->configuration_value = request->wValue;
            USB_Device_Configure();
            usb_device_handle->device_state = USB_DEVICE_STATE_CONFIGURED;
            log_info("Switching control transfer state to IN-STATUS");
            usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_STATUS_IN;
            break;
        default:
            /* do nothing */
            break;
    }
}

static void process_class_interface_request(const USB_Request_t* request)
{
    switch(request->bRequest){
        case USB_HID_SETIDLE:
            log_info("Switching control transfer stage to IN-STATUS");
            usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_STATUS_IN;
            break;
        default:
            /* do nothing */
            break;
    }
}

static void process_standard_interface_request(const USB_Request_t* request)
{
    switch(request->wValue >> 8){
        case USB_DESCRIPTOR_TYPE_HID_REPORT:
            usb_device_handle->ptr_in_buffer = &hid_report_descriptor;
            usb_device_handle->in_data_size = sizeof(hid_report_descriptor);
            log_info("Switching control transfer stage to IN-STATUS");
            usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN;
            break;
        default:
            /* do nothing */
            break;
    }
}

static void process_control_transfer_stage(void)
{
    uint8_t data_size = MIN(usb_device_handle->in_data_size, device_descriptor.bMaxPacketSize0);

    switch(usb_device_handle->control_transfer_stage){
        case USB_CONTROL_STAGE_SETUP:
            /* do nothing */
            break;
        case USB_CONTROL_STAGE_DATA_IN:
            log_info("Processing IN-DATA stage");
            USB_driver.USB_Write_Packet(0, usb_device_handle->ptr_in_buffer, data_size);
            usb_device_handle->in_data_size -= data_size;
            usb_device_handle->ptr_in_buffer += data_size;

            if(usb_device_handle->in_data_size == 0){
                if(data_size == device_descriptor.bMaxPacketSize0){
                    log_info("Switching control stage to IN-DATA ZERO");
                    usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN_ZERO;
                }
                else{
                    log_info("Switching control stage to OUT-STATUS");
                    usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_STATUS_OUT;
                }
            }
            else{
                log_info("Switching control stage to IN-DATA IDLE");
                usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_DATA_IN_IDLE;
            }
            break;
        case USB_CONTROL_STAGE_DATA_IN_IDLE:
            /* do nothing */
            break;
        case USB_CONTROL_STAGE_STATUS_OUT:
            log_info("Switching control stage to SETUP");
            usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_SETUP;
            break;
        case USB_CONTROL_STAGE_STATUS_IN:
            USB_driver.USB_Write_Packet(0, NULL, 0);
            log_info("Switching control transfer stage to SETUP");
            usb_device_handle->control_transfer_stage = USB_CONTROL_STAGE_SETUP;
            break;
        default:
            /* do nothing */
            break;
    }
}

static void write_mouse_report(void)
{
    log_debug("Sending USB HID mouse report");

    HID_Report_t hid_report = {
        .x = 5
    };

    USB_driver.USB_Write_Packet(
        (cfg_descriptor_combination.usb_mouse_endpoint_descriptor.bEndpointAddress & 0x0F),
        &hid_report,
        sizeof(hid_report)
    );
}