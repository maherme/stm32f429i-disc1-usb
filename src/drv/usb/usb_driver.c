/********************************************************************************************************//**
* @file usb_driver.c
*
* @brief File containing the APIs for USB driver.
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
*
* @note
*       For further information about functions refer to the corresponding header file.
**/

#include "usb_driver.h"
#include "logger.h"
#include "stm32f4xx.h"
#include <stdint.h>

/***********************************************************************************************************/
/*                                       Static Function Prototypes                                        */
/***********************************************************************************************************/

/**
 * @brief Function for calculating the address of the structure which contains the registers of a specific
 *        IN endpoint.
 * @param[in] endpoint_number The number of the IN endpoint to access its registers.
 * @return a pointer to the structure contains the registers of a specific IN endpoint.
 */
static inline __attribute__((always_inline)) USB_OTG_INEndpointTypeDef* IN_ENDPOINT(uint8_t endpoint_number);

/**
 * @brief Function for calculating the address of the structure which contains the registers of a specific
 *        OUT endpoint.
 * @param[in] endpoint_number The number of the OUT endpoint to access its registers.
 * @return a pointer to the structure contains the registers of a specific OUT endpoint.
 */
static inline __attribute__((always_inline)) USB_OTG_OUTEndpointTypeDef* OUT_ENDPOINT(uint8_t endpoint_number);

/**
 * @brief Function for getting the FIFO data of an enpoint.
 * @param[in] endpoint_number The number of the endpoint to access its FIFO.
 * @return a pointer to the FIFO data.
 */
static inline __attribute__((always_inline)) __IO uint32_t* FIFO(uint8_t endpoint_number);

/**
 * @brief Function for configuring the endpoint 0
 * @param[in] endpoint_size is the size of the endpoint.
 * @return void
 */
static void USB_Configure_Endpoint0(uint16_t endpoint_size);

/**
 * @brief Function for deconfiguring an endpoint
 * @param[in] endpoint_number is the number of the endpoint to deconfigure.
 * @return void
 */
static void USB_Deconfigure_Endpoint(uint8_t endpoint_number);

/**
 * @brief Function for configuring the RxFIFO of all OUT endpoints.
 * @param[in] size is the size of the largest OUT endpoint in bytes.
 * @return void
 * @note The RxFIFO is shared between all OUT endpoints.
 */
static void USB_Configure_RxFIFO_Size(uint16_t size);

/**
 * @brief Function for configuring the TxFIFO of all IN endpoints.
 * @param[in] endpoint_number is the number of the IN endpoint for configuring its TxFIFO.
 * @param[in] size is the size of the IN endpoint in bytes.
 * @return void
 * @note Any change on any FIFO will update the registers of all TxFIFOs to adapt the start offsets.
 */
static void USB_Configure_TxFIFO_Size(uint8_t endpoint_number, uint16_t size);

/**
 * @brief Function for updating the start addresses of all FIFOs according to the size of each FIFO.
 * @return void
 */
static inline __attribute__((always_inline)) void USB_Refresh_FIFO_Start_Addresses(void);

/**
 * @brief Function for managing the RST interrupt of the USB peripheral.
 * @return void
 */
static inline __attribute__((always_inline)) void USB_RST_Handler(void);

/**
 * @brief Function for managing the enumeration done interrupt of the USB peripheral.
 * @return void
 */
static inline __attribute__((always_inline)) void USB_Enum_Done_Handler(void);

/**
 * @brief Function for managing the rx FIFO non empty interrupt of the USB peripheral.
 * @return void
 */
static inline __attribute__((always_inline)) void USB_RxFIFO_Non_Empty_Handler(void);

/***********************************************************************************************************/
/*                                       Public API Definitions                                            */
/***********************************************************************************************************/

void USB_Init(void)
{
    /* Enable the clock */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_OTGHSEN);

    /* Configure the USB core for running in device mode and using the embedded full-speed PHY */
    MODIFY_REG(
        USB_OTG_HS->GUSBCFG,
        USB_OTG_GUSBCFG_FDMOD | USB_OTG_GUSBCFG_PHYSEL | USB_OTG_GUSBCFG_TRDT,
        USB_OTG_GUSBCFG_FDMOD | USB_OTG_GUSBCFG_PHYSEL | _VAL2FLD(USB_OTG_GUSBCFG_TRDT, 0x09)
    );

    /* Configure for running in full speed mode */
    MODIFY_REG(USB_OTG_HS_DEVICE->DCFG, USB_OTG_DCFG_DSPD, _VAL2FLD(USB_OTG_DCFG_DSPD, 0x03));

    /* Enable VBUS sensing device */
    SET_BIT(USB_OTG_HS->GCCFG, USB_OTG_GCCFG_VBUSBSEN);

    /* Unmask USB core interrupts */
    SET_BIT(
        USB_OTG_HS->GINTMSK,
        USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_SOFM |
        USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_WUIM | USB_OTG_GINTMSK_IEPINT |
        USB_OTG_GINTSTS_OEPINT | USB_OTG_GINTMSK_RXFLVLM
    );

    /* Clear pending interrupts */
    WRITE_REG(USB_OTG_HS->GINTSTS, 0xFFFFFFFF);

    /* Unmask USB global interrupt */
    SET_BIT(USB_OTG_HS->GAHBCFG, USB_OTG_GAHBCFG_GINT);

    /* Unmask tx completed interrupt for all endpoints */
    SET_BIT(USB_OTG_HS_DEVICE->DOEPMSK, USB_OTG_DOEPMSK_XFRCM);
    SET_BIT(USB_OTG_HS_DEVICE->DIEPMSK, USB_OTG_DIEPMSK_XFRCM);
}

void USB_Connect(void)
{
    /* Power the transceiver on */
    SET_BIT(USB_OTG_HS->GCCFG, USB_OTG_GCCFG_PWRDWN);

    /* Connect the device to the bus */
    CLEAR_BIT(USB_OTG_HS_DEVICE->DCTL, USB_OTG_DCTL_SDIS);
}

void USB_Disconnect(void)
{
    /* Disconnect the device to the bus */
    SET_BIT(USB_OTG_HS_DEVICE->DCTL, USB_OTG_DCTL_SDIS);

    /* Power the transceiver off */
    CLEAR_BIT(USB_OTG_HS->GCCFG, USB_OTG_GCCFG_PWRDWN);
}

void USB_Configure_IN_Endpoint(uint8_t endpoint_number,
                               USBEndpointType endpoint_type,
                               uint16_t endpoint_size)
{
    /* Unmask all interrupts of the IN endpoint */
    SET_BIT(USB_OTG_HS_DEVICE->DAINTMSK, 1 << endpoint_number);

    /* Activate the endpoint, set endpoint handshake to NAK (not ready to send data), set DATA0 packet
       configures its type, its maximum packet size and assigns it a TxFIFO */
    MODIFY_REG(
        IN_ENDPOINT(endpoint_number)->DIEPCTL,
        USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_TXFNUM,
        USB_OTG_DIEPCTL_USBAEP | _VAL2FLD(USB_OTG_DIEPCTL_MPSIZ, endpoint_size) | USB_OTG_DIEPCTL_SNAK |
        _VAL2FLD(USB_OTG_DIEPCTL_EPTYP, endpoint_type) | _VAL2FLD(USB_OTG_DIEPCTL_TXFNUM, endpoint_number) |
        USB_OTG_DIEPCTL_SD0PID_SEVNFRM
    );

    USB_Configure_TxFIFO_Size(endpoint_number, endpoint_size);
}

void USB_Read_Packet(void* buffer, uint16_t size)
{
    uint32_t* fifo = (uint32_t*)FIFO(0);

    for(; size >= 4; size -= 4, buffer += 4){
        /* Pops one 32 bit word of data (until there is less than one word remaining) */
        uint32_t data = *fifo;
        *((uint32_t*)buffer) = data;
    }

    if(size > 0){
        /* Pops the last remaining bytes (which are less than one word) */
        uint32_t data = *fifo;
        /* Pop the last remaining bytes (which are less than one word) */
        for(; size > 0; size--, buffer++, data >>= 8){
            /* Store data in the buffer with the correct alignment */
            *(uint8_t*)buffer = 0xFF & data;
        }
    }
}

void USB_Write_Packet(uint8_t endpoint_number, void const* buffer, uint16_t size)
{
    uint32_t* fifo = (uint32_t*)FIFO(endpoint_number);
    USB_OTG_INEndpointTypeDef* in_endpoint = IN_ENDPOINT(endpoint_number);

    /* Configure the tx (1 packet that has size bytes) */
    MODIFY_REG(
        in_endpoint->DIEPTSIZ,
        USB_OTG_DIEPTSIZ_PKTCNT | USB_OTG_DIEPTSIZ_XFRSIZ,
        _VAL2FLD(USB_OTG_DIEPTSIZ_PKTCNT, 1) | _VAL2FLD(USB_OTG_DIEPTSIZ_XFRSIZ, size)
    );

    /* Enable the tx after clearing both STALL and NAK of the endpoint */
    MODIFY_REG(
        in_endpoint->DIEPCTL,
        USB_OTG_DIEPCTL_STALL,
        USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA
    );

    /* Get the size in term of 32 bit words (to avoid integer overflow in the loop) */
    size = (size + 3)/4;

    for(; size > 0; size--, buffer += 4){
        /* Push the data to the TxFIFO */
        *fifo = *(uint32_t*)buffer;
    }
}

void USB_Flush_RxFIFO(void)
{
    SET_BIT(USB_OTG_HS->GRSTCTL, USB_OTG_GRSTCTL_RXFFLSH);
}

void USB_Flush_TxFIFO(uint8_t endpoint_number)
{
    /* Set the number of the TxFIFO to be flushed and then triggers the flush */
    MODIFY_REG(
        USB_OTG_HS->GRSTCTL,
        USB_OTG_GRSTCTL_TXFNUM,
        _VAL2FLD(USB_OTG_GRSTCTL_TXFNUM, endpoint_number) | USB_OTG_GRSTCTL_TXFFLSH
    );
}

void USB_IRQ_Handler(void)
{
    volatile uint32_t irq = USB_OTG_HS_GLOBAL->GINTSTS;

    /* Reset irq */
    if(irq & USB_OTG_GINTSTS_USBRST){
        USB_RST_Handler();
        /* Clear irq */
        SET_BIT(irq, USB_OTG_GINTSTS_USBRST);
    }
    /* Enumeration done irq */
    else if(irq & USB_OTG_GINTSTS_ENUMDNE){
        USB_Enum_Done_Handler();
        /* Clear irq */
        SET_BIT(irq, USB_OTG_GINTSTS_ENUMDNE);
    }
    /* Rx-FIFO non-empty irq */
    else if(irq & USB_OTG_GINTSTS_RXFLVL){
        USB_RxFIFO_Non_Empty_Handler();
        /* Clear irq */
        SET_BIT(irq, USB_OTG_GINTSTS_RXFLVL);
    }
    else if(irq & USB_OTG_GINTSTS_IEPINT){

    }
    else if(irq & USB_OTG_GINTSTS_OEPINT){

    }
    else{
        /* do nothing */
    }
}

/***********************************************************************************************************/
/*                                       Static Function Definitions                                       */
/***********************************************************************************************************/

static inline __attribute__((always_inline)) USB_OTG_INEndpointTypeDef* IN_ENDPOINT(uint8_t endpoint_number)
{
    return(USB_OTG_INEndpointTypeDef*)(USB_OTG_HS_PERIPH_BASE + 
                                       USB_OTG_IN_ENDPOINT_BASE + 
                                       (endpoint_number * 0x20));
}

static inline __attribute__((always_inline)) USB_OTG_OUTEndpointTypeDef* OUT_ENDPOINT(uint8_t endpoint_number)
{
    return(USB_OTG_OUTEndpointTypeDef*)(USB_OTG_HS_PERIPH_BASE +
                                        USB_OTG_OUT_ENDPOINT_BASE +
                                        (endpoint_number * 0x20));
}

static inline __attribute__((always_inline)) __IO uint32_t* FIFO(uint8_t endpoint_number)
{
    return (__IO uint32_t*)(USB_OTG_HS_PERIPH_BASE + USB_OTG_FIFO_BASE + (endpoint_number * 0x1000));
}

static void USB_Configure_Endpoint0(uint16_t endpoint_size)
{
    /* Unmask irq of IN and OUT endpoint0 */
    SET_BIT(USB_OTG_HS_DEVICE->DAINTMSK, (1 << 0) | (1 << 16));

    /* Configure the maximum packet size, activates the endpoint and NAK the endpoint */
    MODIFY_REG(
        IN_ENDPOINT(0)->DIEPCTL,
        USB_OTG_DIEPCTL_MPSIZ,
        USB_OTG_DIEPCTL_USBAEP | _VAL2FLD(USB_OTG_DIEPCTL_MPSIZ, endpoint_size) | USB_OTG_DIEPCTL_SNAK
    );

    /* Clear NAK and enable endpoint data transmission */
    SET_BIT(OUT_ENDPOINT(0)->DOEPCTL, USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK);

    /* 64 bytes is the maximum packet size for full speed USB devices */
    USB_Configure_RxFIFO_Size(64);
    USB_Configure_TxFIFO_Size(0, endpoint_size);
}

static void USB_Deconfigure_Endpoint(uint8_t endpoint_number)
{
    USB_OTG_INEndpointTypeDef* in_endpoint = IN_ENDPOINT(endpoint_number);
    USB_OTG_OUTEndpointTypeDef* out_endpoint = OUT_ENDPOINT(endpoint_number);

    /* Mask all interrupts of the IN and OUT endpoints */
    CLEAR_BIT(USB_OTG_HS_DEVICE->DAINTMSK, (1 << endpoint_number) | (1 << 16 << endpoint_number));

    /* Clear all interrupts of the endpoint */
    SET_BIT(in_endpoint->DIEPINT, 0x29FF);
    SET_BIT(out_endpoint->DOEPINT, 0x715F);

    /* Disable endpoint */
    if(in_endpoint->DIEPCTL & USB_OTG_DIEPCTL_EPENA){
        SET_BIT(in_endpoint->DIEPCTL, USB_OTG_DIEPCTL_EPDIS);
    }

    /* Deactivates the endpoint */
    CLEAR_BIT(in_endpoint->DIEPCTL, USB_OTG_DIEPCTL_USBAEP);

    if(endpoint_number != 0){
        if(out_endpoint->DOEPCTL, USB_OTG_DOEPCTL_EPENA){
            SET_BIT(out_endpoint->DOEPCTL, USB_OTG_DOEPCTL_EPDIS);
        }
        /* Deactivate endpoint */
        CLEAR_BIT(out_endpoint->DOEPCTL, USB_OTG_DOEPCTL_USBAEP);
    }

    /* Flush the FIFOs */
    USB_Flush_TxFIFO(endpoint_number);
    USB_Flush_RxFIFO();
}

static void USB_Configure_RxFIFO_Size(uint16_t size)
{
    /* Space required to save status packets in RxFIFO and gets the size in term of 32 bits */
    size = 10 + (2*((size/4) + 1));

    /* Configure the size of the FIFO */
    MODIFY_REG(USB_OTG_HS->GRXFSIZ, USB_OTG_GRXFSIZ_RXFD, _VAL2FLD(USB_OTG_GRXFSIZ_RXFD, size));

    USB_Refresh_FIFO_Start_Addresses();
}

static void USB_Configure_TxFIFO_Size(uint8_t endpoint_number, uint16_t size)
{
    /* Get FIFO size in term of 32 bit words */
    size = (size + 3)/4;

    /* Configure the size of the TxFIFO */
    if(endpoint_number == 0){
        MODIFY_REG(USB_OTG_HS->DIEPTXF0_HNPTXFSIZ, USB_OTG_TX0FD, _VAL2FLD(USB_OTG_TX0FD, size));
    }
    else{
        MODIFY_REG(
            USB_OTG_HS->DIEPTXF[endpoint_number - 1],
            USB_OTG_NPTXFD,
            _VAL2FLD(USB_OTG_NPTXFD, size)
        );
    }
    USB_Refresh_FIFO_Start_Addresses();
}

static inline __attribute__((always_inline)) void USB_Refresh_FIFO_Start_Addresses(void)
{
    /* First changeable start address begins after the region of RxFIFO */
    uint16_t start_address = _FLD2VAL(USB_OTG_GRXFSIZ_RXFD, USB_OTG_HS->GRXFSIZ)*4;

    /* Update the start address of the TxFIFO */
    MODIFY_REG(USB_OTG_HS->DIEPTXF0_HNPTXFSIZ, USB_OTG_TX0FSA, _VAL2FLD(USB_OTG_TX0FSA, start_address));

    /* Next start address is after where the last TxFIFO ends */
    start_address += _FLD2VAL(USB_OTG_TX0FD, USB_OTG_HS->DIEPTXF0_HNPTXFSIZ)*4;

    /* Update the start address of the rest of TxFIFOs */
    for(uint8_t i = 0; i < (USB_ENDPOINT_COUNT - 1); i++){
        MODIFY_REG(USB_OTG_HS->DIEPTXF[i], USB_OTG_NPTXFSA, _VAL2FLD(USB_OTG_NPTXFSA, start_address));
        start_address += _FLD2VAL(USB_OTG_NPTXFD, USB_OTG_HS->DIEPTXF[i])*4;
    }
}

static inline __attribute__((always_inline)) void USB_RST_Handler(void)
{
    log_info("USB reset signal was detected");

    for(uint8_t i = 0; i <= USB_ENDPOINT_COUNT; i++){
        USB_Deconfigure_Endpoint(i);
    }
}

static inline __attribute__((always_inline)) void USB_Enum_Done_Handler(void)
{
    log_info("USB device speed enumeration done");

    USB_Configure_Endpoint0(8);
}

static inline __attribute__((always_inline)) void USB_RxFIFO_Non_Empty_Handler(void)
{
    /* Pop the status information word from the RxFIFO */
    uint32_t rx_status = USB_OTG_HS_GLOBAL->GRXSTSP;

    /* The endpoint that received the data */
    uint8_t  endpoint_number = _FLD2VAL(USB_OTG_GRXSTSP_EPNUM, rx_status);
    /* The count of bytes in the received packet */
    uint16_t byte_count = _FLD2VAL(USB_OTG_GRXSTSP_BCNT, rx_status);
    /* The status of the received packet */
    uint16_t packet_status = _FLD2VAL(USB_OTG_GRXSTSP_PKTSTS, rx_status);

    switch(packet_status)
    {
        /* SETUP packet (includes data) */
        case 0x06:
            break;
        /* OUT packet (includes data) */
        case 0x02:
            break;
        /* SETUP stage has completed */
        case 0x04:
        /* OUT transfer has completed */
        case 0x03:
            /* Re-enables the tx on teh endpoint */
            SET_BIT(OUT_ENDPOINT(endpoint_number)->DOEPCTL,
                    USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
            break;
        default:
            break;
    }
}