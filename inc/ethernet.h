#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#include <stdint.h>

#define PORTNUM 8888

#ifndef ETHERNET_DATA_BUF_SIZE
	#define ETHERNET_DATA_BUF_SIZE			2048
#endif

void ethernet_init( void );

#endif