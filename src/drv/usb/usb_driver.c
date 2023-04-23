/********************************************************************************************************//**
* @file usb_driver.c
*
* @brief File containing the APIs for USB driver.
*
* Public Functions:
*       - void USB_Init(void)
*       - void USB_Connect(void)
*       - void USB_Disconnect(void)
*
* @note
*       For further information about functions refer to the corresponding header file.
**/

#include "usb_driver.h"
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

static void USB_configure_endpoint0(uint16_t endpoint_size);

static void USB_configure_in_endpoint(uint8_t endpoint_number,
                                      USBEndpointType endpoint_type,
                                      uint16_t endpoint_size);

/**
 * @brief Function for managing the RST interrupt of the USB peripheral.
 * @return void
 */
static inline __attribute__((always_inline)) void USB_RST_Handler(void);

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

void USB_IRQ_Handler(void)
{
    volatile uint32_t irq = USB_OTG_HS_GLOBAL->GINTSTS;

    if(irq & USB_OTG_GINTSTS_USBRST){
        /* Clear irq */
        SET_BIT(irq, USB_OTG_GINTSTS_USBRST);
    }
    else if(irq & USB_OTG_GINTSTS_ENUMDNE){

    }
    else if(irq & USB_OTG_GINTSTS_RXFLVL){

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

static void USB_configure_endpoint0(uint16_t endpoint_size)
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
}

static void USB_configure_in_endpoint(uint8_t endpoint_number,
                                      USBEndpointType endpoint_type,
                                      uint16_t endpoint_size)
{
    /* Unmask all interrupts of the IN endpoint */
    SET_BIT(USB_OTG_HS_DEVICE->DAINTMSK, 1 << endpoint_number);

    /* Activate the endpoint, set endpoint handshake to NAK (not ready to send data), set DATA0 packet
       configures its type, its maximum packet size and assigns it a TxFIFO */
    MODIFY_REG(
        IN_ENDPOINT(endpoint_number)->DIEPCTL,
        USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP,
        USB_OTG_DIEPCTL_USBAEP | _VAL2FLD(USB_OTG_DIEPCTL_MPSIZ, endpoint_size) | USB_OTG_DIEPCTL_SNAK |
        _VAL2FLD(USB_OTG_DIEPCTL_EPTYP, endpoint_type) | USB_OTG_DIEPCTL_SD0PID_SEVNFRM
    );
}

static inline __attribute__((always_inline)) void USB_RST_Handler(void)
{
    for(uint8_t i = 0; i <= USB_ENDPOINT_COUNT; i++){

    }
}