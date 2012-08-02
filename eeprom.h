 
#ifndef _INCLUDE_RW3600_H_
#define _INCLUDE_RW3600_H_ 

#include "linux3600.h"

#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <bcm2835.h>

#define MAXRETRIES          20


/* Generic functions */

int eeprom_read(unsigned char *buf, size_t count);
int eeprom_seek(off_t pos);



void open_weatherstation();

int close_weatherstation();

int read_data(int number,
			  unsigned char *readdata);
			  
int write_data(int address, int number,
			   unsigned char *writedata);


void read_next_byte_seq();
void read_last_byte_seq();

int read_bit();
void write_bit(int bit);
int read_byte();
int write_byte(int byte);
void print_log(int log_level, char* str);

void sleep_short(int milliseconds);
void set_DTR(int val);
void set_RTS(int val);
int get_DSR();
int get_CTS();
long calibrate();
void nanodelay();
#endif /* _INCLUDE_RW3600_H_ */

