/*  open3600  - linux3600 library functions
 *  This file contains the common functions that are unique to
 *  Linux. The entire file is ignored in case of Windows
 *  
 *  Version 0.01
 *  
 *  Control WS3600 weather station
 *  
 *  Copyright 2003-2005, Kenneth Lavrsen, Grzegorz Wisniewski, Sander Eerkes
 *  This program is published under the GNU General Public license
 */

#define DEBUG 0

#include "eeprom.h"

#define TxD RPI_GPIO_P1_11
#define DTR RPI_GPIO_P1_18
#define RTS RPI_GPIO_P1_16
#define DSR RPI_GPIO_P1_13
#define CTS RPI_GPIO_P1_15

/********************************************************************
 * open_weatherstation, Windows version
 *
 ********************************************************************/
void open_weatherstation () {
  long i;
  print_log(1,"open_weatherstation");

//  bcm2835_set_debug(1);

  if (!bcm2835_init())
  {
    printf("\nUnable to open bcm2835 library\n");
    exit(EXIT_FAILURE);
  }

  // set input and outputs
  bcm2835_gpio_fsel(TxD, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(DTR, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(RTS, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(DSR, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(CTS, BCM2835_GPIO_FSEL_INPT);


  printf ("Try to takeover the bus...\n");
  for (i = 0; i < 4*448; i++) {
    // 'U' / 0101 0101 -> TxD (300Baud)
    bcm2835_gpio_write(TxD, LOW);
    delay(3);
    bcm2835_gpio_write(TxD, HIGH);
    delay(3);
  }

  set_DTR(0);
  set_RTS(0);
  i = 0;
  do {
    delay(10);
    i++;
  } while (i < INIT_WAIT && !get_DSR());

  if (i == INIT_WAIT)
  {
    print_log(2,"Connection timeout 1");
    printf ("Connection timeout\n");
    close_weatherstation();
    exit(0);
  }
  i = 0;
  do {
    delay(10);
    i++;
  } while (i < INIT_WAIT && get_DSR());

  if (i != INIT_WAIT) {
    set_RTS(1);
    set_DTR(1);
  } else {
    print_log(2,"Connection timeout 2");
    printf ("Connection timeout\n");
    close_weatherstation();
    exit(0);
  }

  printf ("Takeover the bus...\n");
  for (i = 0; i < 4*448; i++) {
    // 'U' / 0101 0101 -> TxD (300Baud)
    bcm2835_gpio_write(TxD, LOW);
    delay(3);
    bcm2835_gpio_write(TxD, HIGH);
    delay(3);
  }
  printf ("...done!\n");
}


/********************************************************************
 * close_weatherstation, Linux version
 *
 * Input: Handle to the weatherstation (type WEATHERSTATION)
 *
 * Returns: 1 if close was successful, 0 otherwise
 *
 ********************************************************************/
int close_weatherstation()
{
  return bcm2835_close();
}

/********************************************************************
 * set_DTR  
 * Sets or resets DTR signal
 *
 * Inputs:  val - value to set 
 * 
 * Returns nothing
 *
 ********************************************************************/

void set_DTR(int val)
{
  bcm2835_gpio_write(DTR, val == 1 ? HIGH : LOW );
}

/********************************************************************
 * set_RTS  
 * Sets or resets RTS signal
 *
 * Inputs:  val - value to set 
 * 
 * Returns nothing
 *
 ********************************************************************/

void set_RTS(int val)
{
  bcm2835_gpio_write(RTS, val == 1 ? HIGH : LOW );
}

/********************************************************************
 * get_DSR  
 * Checks status of DSR signal
 *
 * Returns: status of DSR signal
 *
 ********************************************************************/

int get_DSR()
{
  uint8_t value = bcm2835_gpio_lev(DSR);
  return value;
}

/********************************************************************
 * get_CTS
 * Checks status of CTS signal
 *
 * Returns: status of CTS signal
 *
 ********************************************************************/

int get_CTS()
{
  uint8_t value = bcm2835_gpio_lev(CTS);
  return value;
}

/* Note: if you see timing issues, maybe you need to adjust this ... */
void nanodelay() {
	delayMicroseconds(10);
}

