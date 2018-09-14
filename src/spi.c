#include "uf2.h"
#include "spi.h"

/*  =========================
 *  ===== Sercom SPI
 *  =========================
*/

void initClock( Sercom *sercom )
{
#if defined(__samd51__)
	uint32_t clk_core;
	uint32_t clk_slow;
	
	if(sercom == SERCOM0)
	{
		clk_core = SERCOM0_GCLK_ID_CORE;
		clk_slow = SERCOM0_GCLK_ID_SLOW;

    NVIC_ClearPendingIRQ(SERCOM0_0_IRQn);
    NVIC_ClearPendingIRQ(SERCOM0_1_IRQn);
    NVIC_ClearPendingIRQ(SERCOM0_2_IRQn);
    NVIC_ClearPendingIRQ(SERCOM0_3_IRQn);

    NVIC_SetPriority (SERCOM0_0_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority */
    NVIC_SetPriority (SERCOM0_1_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM0_2_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM0_3_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    NVIC_EnableIRQ(SERCOM0_0_IRQn);
    NVIC_EnableIRQ(SERCOM0_1_IRQn);
    NVIC_EnableIRQ(SERCOM0_2_IRQn);
    NVIC_EnableIRQ(SERCOM0_3_IRQn);
	}
	else if(sercom == SERCOM1)
	{
		clk_core = SERCOM1_GCLK_ID_CORE;
		clk_slow = SERCOM1_GCLK_ID_SLOW;

    NVIC_ClearPendingIRQ(SERCOM1_0_IRQn);
    NVIC_ClearPendingIRQ(SERCOM1_1_IRQn);
    NVIC_ClearPendingIRQ(SERCOM1_2_IRQn);
    NVIC_ClearPendingIRQ(SERCOM1_3_IRQn);

    NVIC_SetPriority (SERCOM1_0_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority */
    NVIC_SetPriority (SERCOM1_1_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM1_2_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM1_3_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    NVIC_EnableIRQ(SERCOM1_0_IRQn);
    NVIC_EnableIRQ(SERCOM1_1_IRQn);
    NVIC_EnableIRQ(SERCOM1_2_IRQn);
    NVIC_EnableIRQ(SERCOM1_3_IRQn);
	}
	else if(sercom == SERCOM2)
	{
		clk_core = SERCOM2_GCLK_ID_CORE;
		clk_slow = SERCOM2_GCLK_ID_SLOW;

    NVIC_ClearPendingIRQ(SERCOM2_0_IRQn);
    NVIC_ClearPendingIRQ(SERCOM2_1_IRQn);
    NVIC_ClearPendingIRQ(SERCOM2_2_IRQn);
    NVIC_ClearPendingIRQ(SERCOM2_3_IRQn);

    NVIC_SetPriority (SERCOM2_0_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority */
    NVIC_SetPriority (SERCOM2_1_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM2_2_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM2_3_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    NVIC_EnableIRQ(SERCOM2_0_IRQn);
    NVIC_EnableIRQ(SERCOM2_1_IRQn);
    NVIC_EnableIRQ(SERCOM2_2_IRQn);
    NVIC_EnableIRQ(SERCOM2_3_IRQn);
	}
	else if(sercom == SERCOM3)
	{
		clk_core = SERCOM3_GCLK_ID_CORE;
		clk_slow = SERCOM3_GCLK_ID_SLOW;

    NVIC_ClearPendingIRQ(SERCOM3_0_IRQn);
    NVIC_ClearPendingIRQ(SERCOM3_1_IRQn);
    NVIC_ClearPendingIRQ(SERCOM3_2_IRQn);
    NVIC_ClearPendingIRQ(SERCOM3_3_IRQn);

    NVIC_SetPriority (SERCOM3_0_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority */
    NVIC_SetPriority (SERCOM3_1_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM3_2_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM3_3_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    NVIC_EnableIRQ(SERCOM3_0_IRQn);
    NVIC_EnableIRQ(SERCOM3_1_IRQn);
    NVIC_EnableIRQ(SERCOM3_2_IRQn);
    NVIC_EnableIRQ(SERCOM3_3_IRQn);
	}
	else if(sercom == SERCOM4)
	{
		clk_core = SERCOM4_GCLK_ID_CORE;
		clk_slow = SERCOM4_GCLK_ID_SLOW;

    NVIC_ClearPendingIRQ(SERCOM4_0_IRQn);
    NVIC_ClearPendingIRQ(SERCOM4_1_IRQn);
    NVIC_ClearPendingIRQ(SERCOM4_2_IRQn);
    NVIC_ClearPendingIRQ(SERCOM4_3_IRQn);

    NVIC_SetPriority (SERCOM4_0_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority */
    NVIC_SetPriority (SERCOM4_1_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM4_2_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM4_3_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    NVIC_EnableIRQ(SERCOM4_0_IRQn);
    NVIC_EnableIRQ(SERCOM4_1_IRQn);
    NVIC_EnableIRQ(SERCOM4_2_IRQn);
    NVIC_EnableIRQ(SERCOM4_3_IRQn);
	}
	else if(sercom == SERCOM5)
	{
		clk_core = SERCOM5_GCLK_ID_CORE;
		clk_slow = SERCOM5_GCLK_ID_SLOW;

    NVIC_ClearPendingIRQ(SERCOM5_0_IRQn);
    NVIC_ClearPendingIRQ(SERCOM5_1_IRQn);
    NVIC_ClearPendingIRQ(SERCOM5_2_IRQn);
    NVIC_ClearPendingIRQ(SERCOM5_3_IRQn);

    NVIC_SetPriority (SERCOM5_0_IRQn, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority */
    NVIC_SetPriority (SERCOM5_1_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM5_2_IRQn, (1<<__NVIC_PRIO_BITS) - 1); 
    NVIC_SetPriority (SERCOM5_3_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

    NVIC_EnableIRQ(SERCOM5_0_IRQn);
    NVIC_EnableIRQ(SERCOM5_1_IRQn);
    NVIC_EnableIRQ(SERCOM5_2_IRQn);
    NVIC_EnableIRQ(SERCOM5_3_IRQn);
	}
#else

  IRQn_Type IdNvic=PendSV_IRQn ; // Dummy init to intercept potential error later

  uint8_t clockId = 0;
  if(sercom == SERCOM0)
  {
    clockId = GCM_SERCOM0_CORE;
    IdNvic = SERCOM0_IRQn;
  }
  else if(sercom == SERCOM1)
  {
    clockId = GCM_SERCOM1_CORE;
    IdNvic = SERCOM1_IRQn;
  }
  else if(sercom == SERCOM2)
  {
    clockId = GCM_SERCOM2_CORE;
    IdNvic = SERCOM2_IRQn;
  }
  else if(sercom == SERCOM3)
  {
    clockId = GCM_SERCOM3_CORE;
    IdNvic = SERCOM3_IRQn;
  }
#if defined(SERCOM4)
  else if(sercom == SERCOM4)
  {
    clockId = GCM_SERCOM4_CORE;
    IdNvic = SERCOM4_IRQn;
  }
#endif
#if defined(SERCOM5)
  else if(sercom == SERCOM5)
  {
    clockId = GCM_SERCOM5_CORE;
    IdNvic = SERCOM5_IRQn;
  }
#endif

  if ( IdNvic == PendSV_IRQn )
  {
    // We got a problem here
    return ;
  }
#endif

#if defined(__samd51__)
  GCLK->PCHCTRL[clk_core].reg = GCLK_PCHCTRL_GEN_GCLK0_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
  GCLK->PCHCTRL[clk_slow].reg = GCLK_PCHCTRL_GEN_GCLK3_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
  
#else
    // Setting NVIC
  NVIC_ClearPendingIRQ(IdNvic);
  NVIC_SetPriority (IdNvic, (1<<__NVIC_PRIO_BITS) - 1);  /* set Priority */
  NVIC_EnableIRQ(IdNvic);

  //Setting clock
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( clockId ) | // Generic Clock 0 (SERCOMx)
                      GCLK_CLKCTRL_GEN_GCLK0 | // Generic Clock Generator 0 is source
                      GCLK_CLKCTRL_CLKEN ;

  while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
  {
    /* Wait for synchronization */
  }
#endif
}

static uint8_t calculateBaudrateSynchronous(uint32_t baudrate)
{
  return SERCOM_FREQ_REF / (2 * baudrate) - 1;
}

void initSPI( Sercom *sercom, SercomSpiTXPad mosi, SercomRXPad miso, SercomSpiCharSize charSize, SercomDataOrder dataOrder)
{
  resetSPI(sercom);
  initClock(sercom);

#if defined(__samd51__)
	sercom->SPI.CTRLA.reg =	SERCOM_SPI_CTRLA_MODE(0x3) |  //master mode
						SERCOM_SPI_CTRLA_DOPO(mosi) |
						SERCOM_SPI_CTRLA_DIPO(miso) |
						dataOrder << SERCOM_SPI_CTRLA_DORD_Pos;
#else
  //Setting the CTRLA register
  sercom->SPI.CTRLA.reg =	SERCOM_SPI_CTRLA_MODE_SPI_MASTER |
                          SERCOM_SPI_CTRLA_DOPO(mosi) |
                          SERCOM_SPI_CTRLA_DIPO(miso) |
                          dataOrder << SERCOM_SPI_CTRLA_DORD_Pos;
#endif

  //Setting the CTRLB register
  sercom->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_CHSIZE(charSize) |
                          SERCOM_SPI_CTRLB_RXEN;  //Active the SPI receiver.


}

void initSPIClock( Sercom *sercom, SercomSpiClockMode clockMode, uint32_t baudrate)
{
  //Extract data from clockMode
  int cpha, cpol;

  if((clockMode & (0x1ul)) == 0 )
    cpha = 0;
  else
    cpha = 1;

  if((clockMode & (0x2ul)) == 0)
    cpol = 0;
  else
    cpol = 1;

  //Setting the CTRLA register
  sercom->SPI.CTRLA.reg |=  ( cpha << SERCOM_SPI_CTRLA_CPHA_Pos ) |
                            ( cpol << SERCOM_SPI_CTRLA_CPOL_Pos );

  //Synchronous arithmetic
  sercom->SPI.BAUD.reg = calculateBaudrateSynchronous(baudrate);
}

void resetSPI( Sercom *sercom )
{
  //Setting the Software Reset bit to 1
  sercom->SPI.CTRLA.bit.SWRST = 1;

  //Wait both bits Software Reset from CTRLA and SYNCBUSY are equal to 0
  while(sercom->SPI.CTRLA.bit.SWRST || sercom->SPI.SYNCBUSY.bit.SWRST);
}

void enableSPI( Sercom *sercom )
{
  //Setting the enable bit to 1
  sercom->SPI.CTRLA.bit.ENABLE = 1;

  while(sercom->SPI.SYNCBUSY.bit.ENABLE)
  {
    //Waiting then enable bit from SYNCBUSY is equal to 0;
  }
}

void disableSPI( Sercom *sercom )
{
  while(sercom->SPI.SYNCBUSY.bit.ENABLE)
  {
    //Waiting then enable bit from SYNCBUSY is equal to 0;
  }

  //Setting the enable bit to 0
  sercom->SPI.CTRLA.bit.ENABLE = 0;
}

void setDataOrderSPI( Sercom *sercom, SercomDataOrder dataOrder)
{
  //Register enable-protected
  disableSPI(sercom);

  sercom->SPI.CTRLA.bit.DORD = dataOrder;

  enableSPI(sercom);
}

SercomDataOrder getDataOrderSPI( Sercom *sercom )
{
  return (sercom->SPI.CTRLA.bit.DORD ? LSB_FIRST : MSB_FIRST);
}

void setBaudrateSPI( Sercom *sercom, uint8_t divider)
{
  //Can't divide by 0
  if(divider == 0)
    return;

  //Register enable-protected
  disableSPI(sercom);

  sercom->SPI.BAUD.reg = calculateBaudrateSynchronous( SERCOM_FREQ_REF / divider );

  enableSPI(sercom);
}

void setClockModeSPI( Sercom *sercom, SercomSpiClockMode clockMode)
{
  int cpha, cpol;
  if((clockMode & (0x1ul)) == 0)
    cpha = 0;
  else
    cpha = 1;

  if((clockMode & (0x2ul)) == 0)
    cpol = 0;
  else
    cpol = 1;

  //Register enable-protected
  disableSPI(sercom);

  sercom->SPI.CTRLA.bit.CPOL = cpol;
  sercom->SPI.CTRLA.bit.CPHA = cpha;

  enableSPI(sercom);
}

uint8_t transferDataSPI( Sercom *sercom, uint8_t data)
{
  sercom->SPI.DATA.bit.DATA = data; // Writing data into Data register

  while( sercom->SPI.INTFLAG.bit.RXC == 0 )
  {
    // Waiting Complete Reception
  }

  return sercom->SPI.DATA.bit.DATA;  // Reading data
}
