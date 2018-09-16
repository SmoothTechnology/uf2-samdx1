#include "uf2.h"
#include "socket.h"
#include "wizchip_conf.h"
#include "ethernet.h"
#include "dhcp.h"

//////////////////////////////////////////////////
// Socket & Port number definition for Examples //
//////////////////////////////////////////////////

#define DATA_BUF_SIZE   2048
static uint8_t gDATABUF[DATA_BUF_SIZE];
static uint32_t tmpPageBuf[1024/4];

static const char fullVersion[] = "v" SAM_BA_VERSION " [Arduino:XYZ] " __DATE__ " " __TIME__ "\n\r";

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

static uint8_t  destip[4];
static uint16_t destport;

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
    delay(15);

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

void sam_ba_putdata_ethernet(const uint8_t *data, uint16_t len){
    uint16_t sentsize = 0;
    int ret;
    while(sentsize != len)
    {
       ret = sendto(_sock, (uint8_t *)data+sentsize, len-sentsize, destip, destport);
       if(ret < 0)
       {
    	   __BKPT();
       }
       sentsize += ret;
    }
}

static uint32_t current_number;
static uint32_t i;
static int32_t length = 0;
static uint8_t command, *ptr_data, *ptr;
static uint8_t j;
static uint32_t u32tmp;

void sam_ba_monitor_ethernet( void )
{
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

	ptr_data = NULL;
	command = 'z';

	// Start waiting some cmd
	while (1) {
		length = read_udp(_sock, gDATABUF, PORTNUM);
		if (length < 0)
		{
			//TODO: maybe deal with this fail somehow idk
			__BKPT();
		}

		gDATABUF[length] = 0;
		if (length) {
			logwrite("SERIAL:");
			logmsg(data);
			led_signal();
		}
		ptr = gDATABUF;
		for (i = 0; i < length; i++) {
			if (*ptr != 0xff) {
				if (*ptr == '#') {
#if USE_CDC_TERMINAL
					if (b_terminal_mode) {
						sam_ba_putdata_ethernet("\n\r", 2);
					}
#endif
					if (command == 'S') {
						// Check if some data are remaining in the "data" buffer
						if (length > i) {
							// Move current indexes to next avail data
							// (currently ptr points to "#")
							ptr++;
							i++;
							// We need to add first the remaining data of the
							// current buffer already read from usb
							// read a maximum of "current_number" bytes
							u32tmp = (length - i) < current_number ? (length - i) : current_number;
							memcpy(ptr_data, ptr, u32tmp);
							i += u32tmp;
							ptr += u32tmp;
							j = u32tmp;
						}
						// update i with the data read from the buffer
						i--;
						ptr--;
						// Do we expect more data ?
						if (j < current_number)
							cdc_read_buf_xmd(ptr_data, current_number - j);

						__asm("nop");
					} else if (command == 'R') {
						//TODO: this
						//sam_ba_putdata_ethernet_xmd(ptr_data, current_number);
					} else if (command == 'O') {
						*ptr_data = (char)current_number;
					} else if (command == 'H') {
						*((uint16_t *)(void *)ptr_data) = (uint16_t)current_number;
					} else if (command == 'W') {
						// detect BOSSA resetting us
						if ((uint32_t)ptr_data == 0xE000ED0C)
							RGBLED_set_color(COLOR_LEAVE);
						*((int *)(void *)ptr_data) = current_number;
					} else if (command == 'o') {
						sam_ba_putdata_ethernet(ptr_data, 1);
					} else if (command == 'h') {
						current_number = *((uint16_t *)(void *)ptr_data);
						sam_ba_putdata_ethernet((uint8_t *)&current_number, 2);
					} else if (command == 'w') {
						current_number = *((uint32_t *)(void *)ptr_data);
						sam_ba_putdata_ethernet((uint8_t *)&current_number, 4);
					} else if (command == 'G') {
						call_applet(current_number);
						if (b_sam_ba_interface_usart) {
							sam_ba_putdata_ethernet("\x06", 1);
						}
					} else if (command == 'T') {
#if USE_CDC_TERMINAL
						b_terminal_mode = 1;
						sam_ba_putdata_ethernet("\n\r", 2);
#endif
					} else if (command == 'N') {
#if USE_CDC_TERMINAL
						if (b_terminal_mode == 0) {
							sam_ba_putdata_ethernet("\n\r", 2);
						}
						b_terminal_mode = 0;
#endif
					} else if (command == 'V') {
						sam_ba_putdata_ethernet(fullVersion, sizeof(fullVersion));
					} else if (command == 'X') {
						// Syntax: X[ADDR]#
						// Erase the flash memory starting from ADDR to the end
						// of flash.

						flash_erase_to_end((uint32_t *) current_number);

						// Notify command completed
						sam_ba_putdata_ethernet("X\n\r", 3);
					} else if (command == 'Y') {
						// This command writes the content of a buffer in SRAM
						// into flash memory.

						// Syntax: Y[ADDR],[SIZE]#[DATA]

						ptr++; //move past the #
						memcpy(tmpPageBuf, ptr, current_number); //sort of a mem waste but needs to be word aligned

						flash_write_words((void *)ptr_data, tmpPageBuf, current_number / 4);

						// Notify command completed
						sam_ba_putdata_ethernet("Y\n\r", 3);
					} else if (command == 'Z') {
						// This command calculate CRC for a given area of
						// memory.
						// It's useful to quickly check if a transfer has been
						// done
						// successfully.

						// Syntax: Z[START_ADDR],[SIZE]#
						// Returns: Z[CRC]#

						uint8_t *data = (uint8_t *)ptr_data;
						uint32_t size = current_number;
						uint16_t crc = 0;
						uint32_t i = 0;
						for (i = 0; i < size; i++)
							crc = add_crc(*data++, crc);

						//TODO: write this to the NVM user row

						// TODO: Send response
						/*
						sam_ba_putdata_ethernet("Z", 1);
						put_uint32(crc);
						sam_ba_putdata_ethernet("#\n\r", 3);
						*/
					}

					command = 'z';
					current_number = 0;
#if USE_CDC_TERMINAL
					if (b_terminal_mode) {
						sam_ba_putdata_ethernet(">", 1);
					}
#endif
				} else {
					if (('0' <= *ptr) && (*ptr <= '9')) {
						current_number = (current_number << 4) | (*ptr - '0');

					} else if (('A' <= *ptr) && (*ptr <= 'F')) {
						current_number = (current_number << 4) | (*ptr - 'A' + 0xa);

					} else if (('a' <= *ptr) && (*ptr <= 'f')) {
						current_number = (current_number << 4) | (*ptr - 'a' + 0xa);

					} else if (*ptr == ',') {
						ptr_data = (uint8_t *)current_number;
						current_number = 0;

					} else {
						command = *ptr;
						current_number = 0;
					}
				}
				ptr++;
			}
		}
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
