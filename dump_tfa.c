/* vim:set expandtab! ts=4: */

/*  open3600 - dump3600.c
 *
 *  Version 1.10
 *
 *  Control WS3600 weather station
 *
 *  Copyright 2023-2025, Kenneth Lavrsen, Grzegorz Wisniewski, Sander Eerkes
 *  This program is published under the GNU General Public license
 */

#include "eeprom.h"
#include <time.h>
#include <unistd.h>

#define MAX_RETRIES 10
#define BUFSIZE 32768

void print_usage() {
	fprintf(stderr, "Usage: dump_tfa <dumpfile>\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	FILE *fileptr;
	unsigned char data[BUFSIZE];

	int start_adr, len;
	int block_len = 1000;
	int retries = 0;
	char* filename;

	if (argc != 2) {
		fprintf(stderr, "E: no dump file specified.\n");
		print_usage();
	}

	if (argc >= 3) {
		filename = argv[2];
	} else {
		// generate filename based on current time
		time_t t;
		struct tm *tm;
		t = time(NULL);
		tm = localtime(&t);
		if (tm == NULL) {
			perror("localtime");
			exit(EXIT_FAILURE);
		}
		filename = malloc(50);
		sprintf(filename, "tfa.dump.");
		strftime(filename+strlen(filename), sizeof(filename)-strlen(filename), "%Y%m%d.%H%M", tm);
	}

	// need root for (timing) portio
	if (geteuid() != 0) {
		fprintf(stderr, "E: this program needs root privileges to do direct port I/O.\n");
		exit(EXIT_FAILURE);
	}

	// Setup 
	open_weatherstation();

	// Setup file
	fileptr = fopen(filename, "w");
	if (fileptr == NULL) {
		printf("Cannot open file %s\n", filename);
		exit(EXIT_FAILURE);
	}

	// Start.
	start_adr = 0;
	len = 0x7FFF; // 1802*3; //(0x7ef4+259) - start_adr;
	printf("Dumping %d bytes to %s.\n", len, filename);
	memset(data, 0xAA, BUFSIZE);

	while (start_adr < len) {
		int got_len;
		int this_len = block_len;
		if (start_adr + block_len > len)
			this_len = len-start_adr;

		printf("   ... reading %d bytes beginning from %d\n", this_len, start_adr);

		nanodelay();
		eeprom_seek(start_adr);
		got_len = eeprom_read(data+start_adr, this_len);
		if (got_len != this_len) {
			if (got_len == -1 && retries < MAX_RETRIES) {
				retries++;
				fprintf(stderr, "W: eeprom ack failed, retrying read (retries left: %d).\n", MAX_RETRIES-retries);
				close_weatherstation();
				open_weatherstation();
				continue;
			}
			printf("   >>> got     %d bytes\n", got_len);
			fprintf(stderr, "E: got less than requested bytes, dump is probably unusable.\n");
			break;
		}

		start_adr += block_len;
		retries = 0;
	}

	fwrite(data, len, 1, fileptr);

	close_weatherstation();
	fclose(fileptr);
	return(0);
}

