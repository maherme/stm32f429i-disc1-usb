/********************************************************************************************************//**
* @file usb_driver.h
*
* @brief Header file containing the prototypes of the APIs for USB driver.
*
* Public Functions:
*       - void USB_Init(void)
*       - void USB_Connect(void)
*       - void USB_Disconnect(void)
*       - void USB_Configure_IN_Endpoint(uint8_t endpoint_number,
*                                        USBEndpointType endpoint_type,
*                                        uint16_t endpoint_size)
*       - void USB_Read_Packet(void* buffer, uint16_t size)
*       - void USB_Write_Packet(uint8_t endpoint_number, void const* buffer, uint16_t size)
*       - void USB_Flush_RxFIFO(void)
*       - void USB_Flush_TxFIFO(uint8_t endpoint_number)
*/

#ifndef USB_DRIVER_H
#define USB_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

#define USB_OTG_HS_GLOBAL ((USB_OTG_GlobalTypeDef*)(USB_OTG_HS_PERIPH_BASE + USB_OTG_GLOBAL_BASE))
#define USB_OTG_HS_DEVICE ((USB_OTG_DeviceTypeDef*)(USB_OTG_HS_PERIPH_BASE + USB_OTG_DEVICE_BASE))
#define USB_OTG_HS_PCGCCTL ((uint32_t*)(USB_OTG_HS_PERIPH_BASE + USB_OTG_PCGCCTL_BASE))

/* @brief Number of IN or OUT endpoints */
#define USB_ENDPOINT_COUNT      6

/**
 * @brief List of endpoint types.
 */
typedef enum USBEndpointType
{
    USB_ENDPOINT_TYPE_CONTROL,
    USB_ENDPOINT_TYPE_ISOCHRONOUS,
    USB_ENDPOINT_TYPE_BULK,
    USB_ENDPOINT_TYPE_INTERRUPT
}USBEndpointType;

/**
 * @brief Structure for managing public APIs of USB driver.
 */
typedef struct
{
    void(*USB_Init)(void);
    void(*USB_Connect)(void);
    void(*USB_Disconnect)(void);
    void(*USB_Flush_RxFIFO)(void);
    void(*USB_Flush_TxFIFO)(uint8_t endpoint_number);
    void(*USB_Configure_IN_Endpoint)(uint8_t endpoint_number,
                                    USBEndpointType endpoint_type,
                                    uint16_t endpoint_size);
    void(*USB_Read_Packet)(void* buffer, uint16_t size);
    void(*USB_Write_Packet)(uint8_t endpoint_number, void const* buffer, uint16_t size);
}USB_Driver_t;

/***********************************************************************************************************/
/*                                       APIs Supported                                                    */
/***********************************************************************************************************/

/**
 * @brief Function for initializing the USB peripheral.
 * @return void.
 */
void USB_Init(void);

/**
 * @brief Function for connecting the USB peripheral to the bus.
 * @return void.
 */
void USB_Connect(void);

/**
 * @brief Function for disconnecting the USB peripheral to the bus.
 * @return void.
 */
void USB_Disconnect(void);

/**
 * @brief Function for configuring an IN endpoint
 * @param[in] endpoint_number is the number of the endpoint to configure.
 * @param[in] endpoint_type is the type of endpoint to configure.
 * @param[in] endpoint_size is the size of the endpoint to configure.
 * @return void
 */
void USB_Configure_IN_Endpoint(uint8_t endpoint_number,
                               USBEndpointType endpoint_type,
                               uint16_t endpoint_size);

/**
 * @brief Function for popping data from the RxFIFO and storing it in a buffer.
 * @param[in] buffer is a pointer to a buffer, in which the popped data will be stored.
 * @param[in] size is the counting of bytes to be popped from the dedicated RxFIFO memory.
 * @return void
 */
void USB_Read_Packet(void* buffer, uint16_t size);

/**
 * @brief Function for pushing a packet into the TxFIFO of an IN endpoint.
 * @param[in] endpoint_number is the number of the endpoint to which the data will be written.
 * @param[in] buffer is a pointer to the buffer containing the data to be written to the endpoint.
 * @param[in] size is the size of data to be written in bytes.
 * @return void
 */
void USB_Write_Packet(uint8_t endpoint_number, void const* buffer, uint16_t size);

/**
 * @brief Function for flushing the RxFIFO of all OUT endpoints.
 * @return void
 */
void USB_Flush_RxFIFO(void);

/**
 * @brief Function for flushing the TxFIFO of an IN endpoint.
 * @param[in] endpoint_number is the number of an IN endpoint to flush its FIFO.
 * @return void
 */
void USB_Flush_TxFIFO(uint8_t endpoint_number);

#endif /* USB_DRIVER_H */