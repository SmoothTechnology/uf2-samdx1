#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define VENDOR_NAME "Smooth Technology"
#define PRODUCT_NAME "PoE Playground"
#define VOLUME_LABEL "POEBOOT"
#define INDEX_URL "http://smooth.technology"
#define BOARD_ID "SAMD51J20A-poe-v0"

#define USB_VID 0x239A
#define USB_PID 0x0021

#define LED_PIN PIN_PA16

#define BOARD_NEOPIXEL_PIN PIN_PB01
#define BOARD_NEOPIXEL_COUNT 1

#define BOOT_USART_MODULE                 SERCOM3
#define BOOT_USART_MASK                   APBAMASK
#define BOOT_USART_BUS_CLOCK_INDEX        MCLK_APBBMASK_SERCOM3
#define BOOT_USART_PAD_SETTINGS           UART_RX_PAD1_TX_PAD0
#define BOOT_USART_PAD3                   PINMUX_UNUSED
#define BOOT_USART_PAD2                   PINMUX_UNUSED
#define BOOT_USART_PAD1                   PINMUX_PA22C_SERCOM3_PAD0
#define BOOT_USART_PAD0                   PINMUX_PA23C_SERCOM3_PAD1
#define BOOT_GCLK_ID_CORE                 SERCOM3_GCLK_ID_CORE
#define BOOT_GCLK_ID_SLOW                 SERCOM3_GCLK_ID_SLOW

//SPI Defs
#define SPI_SERCOM SERCOM2

#define SPI_MASTER_CLOCK_INIT MCLK->APBBMASK.reg |= MCLK_APBBMASK_SERCOM2

#define SPI_MOSI PINMUX_PA12C_SERCOM2_PAD0
#define SPI_MISO PINMUX_PA14C_SERCOM2_PAD2
#define SPI_SCLK PINMUX_PA13C_SERCOM2_PAD1

#define SPI_TXCONFIG SPI_PAD_0_SCK_1
#define SPI_RXCONFIG SERCOM_RX_PAD_2

#define WIZ_SS PIN_PA27
#define WIZ_RST PIN_PB07

#endif
