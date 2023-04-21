/********************************************************************************************************//**
* @file usb_driver.c
*
* @brief File containing the APIs for USB driver.
*
* Public Functions:
*
* @note
*       For further information about functions refer to the corresponding header file.
**/

#include "usb_driver.h"
#include "stm32f4xx.h"

/***********************************************************************************************************/
/*                                       Public API Definitions                                            */
/***********************************************************************************************************/

void GPIO_Init(void)
{
    /* Enable GPIOB Clock */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);

    /* Set alternate function for PB14 (-) and PB15 (+) */
    MODIFY_REG(
        GPIOB->AFR[1],
        GPIO_AFRH_AFSEL14 | GPIO_AFRH_AFSEL15,
        _VAL2FLD(GPIO_AFRH_AFSEL14, 0xC) | _VAL2FLD(GPIO_AFRH_AFSEL15, 0xC)
    );

    /* Configure USB pins to work in alternate function mode */
    MODIFY_REG(
        GPIOB->MODER,
        GPIO_MODER_MODER14 | GPIO_MODER_MODER15,
        _VAL2FLD(GPIO_MODER_MODER14, 2) | _VAL2FLD(GPIO_MODER_MODER15, 2)
    );
}

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