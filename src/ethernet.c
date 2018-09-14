#include "uf2.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "ethernet.h"
#include "dhcp.h"

//////////////////////////////////////////////////
// Socket & Port number definition for Examples //
//////////////////////////////////////////////////

#define DATA_BUF_SIZE   2048
uint8_t gDATABUF[DATA_BUF_SIZE];

wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
                            .ip = {192, 168, 1, 177}, //TODO: the IP (and MAC?) address should be loaded from flash USER PAGE
                            .sn = {255, 255, 255, 0},
                            .gw = {192, 168, 1, 1},
                            .dns = {0, 0, 0, 0},
                            .dhcp = NETINFO_STATIC };

////////////////
// DHCP client//
////////////////
#define MY_MAX_DHCP_RETRY			2
static uint8_t my_dhcp_retry = 0;

static uint8_t _sock = 0;

void  wizchip_select(void)
{
	PINOP(WIZ_SS, OUTCLR);
}

void  wizchip_deselect(void)
{
	PINOP(WIZ_SS, OUTSET);
}

uint8_t wizchip_read( void )
{
	return transferDataSPI( SPI_SERCOM, 0);
}

void  wizchip_write(uint8_t wb)
{
	transferDataSPI( SPI_SERCOM, wb);
}

static void W5500_Init( void )
{
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };

    PINOP(WIZ_SS, DIRSET);
    PINOP(WIZ_SS, OUTSET);

    //reset the wiz
    PINOP(WIZ_RST, DIRSET);
    PINOP(WIZ_RST, OUTCLR);
    delay(15);
    PINOP(WIZ_RST, OUTSET);

	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);

	/* wizchip initialize*/
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1) {
		//printf("WIZCHIP Initialized fail.\r\n");
        __BKPT();
		while (1);
	}
}

int32_t read_udp(uint8_t sn, uint8_t* buf, uint16_t port)
{
   int32_t  ret;
   uint16_t size;
   uint8_t  destip[4];
   uint16_t destport;

   switch(getSn_SR(sn))
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(sn)) > 0)
         {
            if(size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
            ret = recvfrom(sn, buf, size, destip, (uint16_t*)&destport);
            if(ret <= 0)
            {
            	__BKPT();
               return ret;
            }
            size = (uint16_t) ret;

#if LOOPBACK
            uint16_t sentsize = 0;
            while(sentsize != size)
            {
               ret = sendto(sn, buf+sentsize, size-sentsize, destip, destport);
               if(ret < 0)
               {
            	   __BKPT();
                  return ret;
               }
               sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
#endif
            return ret;
         }
         break;
      case SOCK_CLOSED:
         if((ret = socket(sn, Sn_MR_UDP, port, 0x00)) != sn)
            return ret;
         break;
      default :
         break;
   }
   return 0;
}

void sam_ba_monitor_ethernet( void )
{
	uint32_t ret;
	/* DHCP */
	/* DHCP IP allocation and check the DHCP lease time (for IP renewal) */
	if(gWIZNETINFO.dhcp == NETINFO_DHCP)
	{
		switch(DHCP_run())
		{
			case DHCP_IP_ASSIGN:
			case DHCP_IP_CHANGED:
				/* If this block empty, act with default_ip_assign & default_ip_update */
				//
				// This example calls my_ip_assign in the two case.
				//
				// Add to ...
				//
				break;
			case DHCP_IP_LEASED:
				//
				// TODO: insert user's code here
				__BKPT();
				//
				break;
			case DHCP_FAILED:
				/* ===== Example pseudo code =====  */
				// The below code can be replaced your code or omitted.
				// if omitted, retry to process DHCP
				my_dhcp_retry++;
				if(my_dhcp_retry > MY_MAX_DHCP_RETRY)
				{
					gWIZNETINFO.dhcp = NETINFO_STATIC;
					DHCP_stop();      // if restart, recall DHCP_init()
					__BKPT();
					my_dhcp_retry = 0;
				}
				break;
			default:
				break;
		}
	}

	if ((ret = read_udp(_sock, gDATABUF, PORTNUM)) < 0) // TCP server loopback test
	{
		__BKPT();
	}

	if(ret > 0){
		//TODO: process commands here
		__BKPT();
	}
}

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

    SPI_MASTER_CLOCK_INIT;

    disableSPI(SPI_SERCOM);

    initSPI(SPI_SERCOM, SPI_TXCONFIG, SPI_RXCONFIG, SPI_CHAR_SIZE_8_BITS, MSB_FIRST);
    initSPIClock(SPI_SERCOM, SERCOM_SPI_MODE_0, 8000000);

    enableSPI(SPI_SERCOM);

    W5500_Init();
    delay(15);
    ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);

    //initialize UDP
    for (int i = 0; i < 8; i++) {
        uint8_t s = getSn_SR(i);
        if (s == SOCK_CLOSED || s == SOCK_FIN_WAIT) {
            _sock = i;
            break;
        }
    }

    delay(15);
    if(socket(_sock, Sn_MR_UDP, PORTNUM, 0) < 0)
    	__BKPT();
}
