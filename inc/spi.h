#ifndef _SPI_H
#define _SPI_H

#include "uf2.h"

typedef enum
{
  SPI_SLAVE_OPERATION = 0x2u,
  SPI_MASTER_OPERATION = 0x3u
} SercomSpiMode;

enum
{
  WIRE_UNKNOWN_STATE = 0x0ul,
  WIRE_IDLE_STATE,
  WIRE_OWNER_STATE,
  WIRE_BUSY_STATE
};

typedef enum
{
  SERCOM_EVEN_PARITY = 0,
  SERCOM_ODD_PARITY,
  SERCOM_NO_PARITY
} SercomParityMode;

typedef enum
{
  SERCOM_STOP_BIT_1 = 0,
  SERCOM_STOP_BITS_2
} SercomNumberStopBit;

typedef enum
{
  MSB_FIRST = 0,
  LSB_FIRST
} SercomDataOrder;

typedef enum
{
  SERCOM_RX_PAD_0 = 0,
  SERCOM_RX_PAD_1,
  SERCOM_RX_PAD_2,
  SERCOM_RX_PAD_3
} SercomRXPad;

typedef enum
{
  SERCOM_SPI_MODE_0 = 0,  // CPOL : 0  | CPHA : 0
  SERCOM_SPI_MODE_1,    // CPOL : 0  | CPHA : 1
  SERCOM_SPI_MODE_2,    // CPOL : 1  | CPHA : 0
  SERCOM_SPI_MODE_3   // CPOL : 1  | CPHA : 1
} SercomSpiClockMode;

typedef enum
{
  SPI_PAD_0_SCK_1 = 0,
  SPI_PAD_2_SCK_3,
  SPI_PAD_3_SCK_1,
  SPI_PAD_0_SCK_3
} SercomSpiTXPad;

typedef enum
{
  SPI_CHAR_SIZE_8_BITS = 0x0ul,
  SPI_CHAR_SIZE_9_BITS
} SercomSpiCharSize;

#define SERCOM_FREQ_REF 48000000

void initSPI( Sercom * sercom, SercomSpiTXPad mosi, SercomRXPad miso, SercomSpiCharSize charSize, SercomDataOrder dataOrder) ;
void initSPIClock( Sercom * sercom, SercomSpiClockMode clockMode, uint32_t baudrate) ;

void resetSPI( Sercom * sercom ) ;
void enableSPI( Sercom * sercom ) ;
void disableSPI( Sercom * sercom ) ;
void setDataOrderSPI( Sercom * sercom, SercomDataOrder dataOrder) ;
SercomDataOrder getDataOrderSPI( Sercom * sercom ) ;
void setBaudrateSPI( Sercom * sercom, uint8_t divider) ;
void setClockModeSPI( Sercom * sercom, SercomSpiClockMode clockMode) ;
uint8_t transferDataSPI( Sercom *sercom, uint8_t data) ;

#endif