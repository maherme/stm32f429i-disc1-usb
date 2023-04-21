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
        USB_OTG_GINTMSK_RXFLVLM
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