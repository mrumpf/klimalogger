/*  open3600  - rw3600.c library functions
 *  This is a library of functions common to Linux and Windows
 *  
 *  Version 0.10
 *  
 *  Control WS3600 weather station
 *  
 *  Copyright 2003-2005, Kenneth Lavrsen, Grzegorz Wisniewski, Sander Eerkes
 *  This program is published under the GNU General Public license
 */

#include "eeprom.h"

/********************************************************************
 * read_data reads data from the WS2300 based on a given address,
 * number of data read, and a an already open serial port
 *
 * Inputs:  number - number of bytes to read, max value 15
 *
 * Output:  readdata - pointer to an array of chars containing
 *                     the just read data, not zero terminated
 * 
 * Returns: number of bytes read, -1 if failed
 *
 ********************************************************************/
int eeprom_read(unsigned char *buf, size_t count) {
  unsigned char command = 0xa1;
  int i;

  if (!write_byte(command))
    return -1;

  for (i = 0; i < count; i++) {
    buf[i] = read_byte();
    if (i + 1 < count)
      read_next_byte_seq();
    //printf("%i\n",readdata[i]);
  }

  read_last_byte_seq();
  return i;
}

int eeprom_seek(off_t pos) {
  return write_data(pos, 0, NULL);
}


/********************************************************************
 * write_data writes data to the WS2300.
 * It can both write nibbles and set/unset bits
 *
 * Inputs:      address (interger - 16 bit)
 *              number - number of nibbles to be written/changed
 *                       must 1 for bit modes (SETBIT and UNSETBIT)
 *                       max 80 for nibble mode (WRITENIB)
 *              writedata - pointer to an array of chars containing
 *                          data to write, not zero terminated
 *                          data must be in hex - one digit per byte
 *                          If bit mode value must be 0-3 and only
 *                          the first byte can be used.
 * 
 * Output:      commanddata - pointer to an array of chars containing
 *                            the commands that were sent to the station
 *
 * Returns:     number of bytes written, -1 if failed
 *
 ********************************************************************/
int write_data(int address, int number, unsigned char *writedata) {
  unsigned char command = 0xa0;
  int i = -1;
  
  write_byte(command);
  write_byte(address/256);
  write_byte(address%256);
  
  if (writedata!=NULL) {
    for (i = 0; i < number; i++)
    {
      write_byte(writedata[i]);
    }
  }
  
  set_DTR(0);
  nanodelay();
  set_RTS(0);
  nanodelay();
  set_RTS(1);
  nanodelay();
  set_DTR(1);
  nanodelay();
  set_RTS(0);
  nanodelay();
  
//return -1 for errors
  return i;
}

void read_next_byte_seq() {
  print_log(3,"read_next_byte_seq");
  write_bit(0);
  set_RTS(0);
  nanodelay();
}

void read_last_byte_seq() {
  print_log(3,"read_last_byte_seq");
  set_RTS(1);
  nanodelay();
  set_DTR(0);
  nanodelay();
  set_RTS(0);
  nanodelay();
  set_RTS(1);
  nanodelay();
  set_DTR(1);
  nanodelay();
  set_RTS(0);
  nanodelay();
}

/********************************************************************
 * read_bit  
 * Reads one bit from the COM
 *
 * Returns: bit read from the COM
 *
 ********************************************************************/

int read_bit() {
  int bit_value;
  char str[20];
  
  set_DTR(0);
  nanodelay();
  bit_value = get_CTS();
  nanodelay();
  set_DTR(1);
  nanodelay();
  sprintf(str,"Read bit %i",!bit_value);
  print_log(4,str);
  
  return !bit_value;
}

/********************************************************************
 * write_bit  
 * Writes one bit to the COM
 *
 * Inputs:  bit - bit to write 
 * 
 * Returns: nothing
 *
 ********************************************************************/
void write_bit(int bit) {
  char str[20];
  
  set_RTS(!bit);
  nanodelay();
  set_DTR(0);
  nanodelay();
  set_DTR(1);
	
  sprintf(str,"Write bit %i",bit);
  print_log(4,str);
}



/********************************************************************
 * read_byte  
 * Reads one byte from the COM
 *
 * Returns: byte read from the COM
 *
 ********************************************************************/
int read_byte() {
  int byte = 0;
  int i;
  char str[20];
  
  for (i = 0; i < 8; i++)
  {
    byte *= 2;
    byte += read_bit();
  }
  sprintf(str,"Read byte %i",byte);
  print_log(3,str);
  
  return byte;
}

/********************************************************************
 * write_byte  
 * Writes one byte to the COM
 *
 * Inputs:  byte - byte to write 
 * 
 * Returns: nothing
 *
 ********************************************************************/
int write_byte(int byte) {
  int status;
  int i;
  char str[20];

  sprintf(str,"Writing byte %i",byte);
  print_log(3,str);

  for (i = 0; i < 8; i++)
  {
    write_bit(byte & 0x80);
    byte <<= 1;
    byte &= 0xff;
  }

  set_RTS(0);
  nanodelay();
  status = get_CTS();
  //TODO: checking value of status, error routine
  nanodelay();
  set_DTR(0);
  nanodelay();
  set_DTR(1);
  nanodelay();
  if (status)
    return 1;
  else
    return 0;
}

/********************************************************************
 * log
 ********************************************************************/
void print_log(int log_level, char* str) {
//  if (log_level < 0)
    fprintf(stderr,"%s\n",str);
}
