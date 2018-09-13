#include "uf2.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "ethernet.h"

static uint8_t _sock = 0;

void ethernet_init( void )
{
    //init spi flash
    uint32_t port;
    uint8_t pin;

    /* Mask 6th bit in pin number to check whether it is greater than 32
     * i.e., PORTB pin */
    port = (SPI_MISO & 0x200000) >> 21;
    pin = SPI_MISO >> 16;
    PORT->Group[port].PINCFG[(pin - (port * 32))].bit.PMUXEN = 1;
    PORT->Group[port].PMUX[(pin - (port * 32)) / 2].reg &= ~(0xF << (4 * (pin & 0x01u)));
    PORT->Group[port].PMUX[(pin - (port * 32)) / 2].reg |= (SPI_MISO & 0xFF)
                                                           << (4 * (pin & 0x01u));

    port = (SPI_MOSI & 0x200000) >> 21;
    pin = SPI_MOSI >> 16;
    PORT->Group[port].PINCFG[(pin - (port * 32))].bit.PMUXEN = 1;
    PORT->Group[port].PMUX[(pin - (port * 32)) / 2].reg &= ~(0xF << (4 * (pin & 0x01u)));
    PORT->Group[port].PMUX[(pin - (port * 32)) / 2].reg |= (SPI_MOSI & 0xFF)
                                                           << (4 * (pin & 0x01u));

    port = (SPI_SCLK & 0x200000) >> 21;
    pin = SPI_SCLK >> 16;
    PORT->Group[port].PINCFG[(pin - (port * 32))].bit.PMUXEN = 1;
    PORT->Group[port].PMUX[(pin - (port * 32)) / 2].reg &= ~(0xF << (4 * (pin & 0x01u)));
    PORT->Group[port].PMUX[(pin - (port * 32)) / 2].reg |= (SPI_SCLK & 0xFF)
                                                           << (4 * (pin & 0x01u));

    PINOP(WIZ_SS, DIRSET);
    PINOP(WIZ_SS, OUTSET);

    SPI_MASTER_CLOCK_INIT;

    disableSPI(SPI_SERCOM);

    initSPI(SPI_SERCOM, SPI_TXCONFIG, SPI_RXCONFIG, SPI_CHAR_SIZE_8_BITS, MSB_FIRST);
    initSPIClock(SPI_SERCOM, SERCOM_SPI_MODE_0, 8000000);

    enableSPI(SPI_SERCOM);

    //unreset the wiz
    PINOP(WIZ_RST, DIRSET);
    PINOP(WIZ_RST, OUTSET);

    for (int i = 0; i < 8; i++) {
        uint8_t s = getSn_SR(i);
        if (s == SOCK_CLOSED || s == SOCK_FIN_WAIT) {
            _sock = i;
            break;
        }
    }

    socket(_sock, SOCK_UDP, PORTNUM, 0);
}