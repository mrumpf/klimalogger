

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	FILE *fileptr;
	unsigned char data[32768];
	int sensors;
	int block_size;

	int i;
	int len;

	int data_offset = 0x64;

	fileptr = fopen("tfa.dump", "r");
	if (fileptr == NULL) {
		printf("Cannot open file %s\n","tfa.dump");
		return 1;
	}

	len = fread(data, 1, 32768, fileptr);
	printf("Read %d bytes.\n", len);

	sensors = data[0x0C];
	printf("Found %d external sensors.\n", sensors);
	sensors++;
	printf(" ==== %d total sensors.\n", sensors);

	if (sensors != 6) {
		printf("Sorry, I don't understand the data, if there are other than \n");
		printf(" 5 external sensors configured!\n");
		return 2;
	}
	//block_size = 5 + 

	for (i=0; i<=(int)(len/15);i++) {
		unsigned char* ptr = data + (i*15) + data_offset;
		printf("%02x:%02x %02x.%02x.20%02x ",
			ptr[1], ptr[0], ptr[2], ptr[3], ptr[4]);
		int f_in, f_1, f_2;
		int t_in, t_1, t_2;
		t_in = (ptr[5] >> 4)*10 + (ptr[5]&0x0F) + ((ptr[6] & 0x0F))*100;
		t_in -= 300;
		t_1 = (ptr[7] & 0x0F)*10 + (ptr[7] >> 4)*100 + (ptr[6] >> 4);
		t_1 -= 300;
		f_in = (ptr[8] & 0x0F) + ((ptr[8] & 0xF0) >> 4) *10;
		f_1 = (ptr[9] & 0x0F) + ((ptr[9] & 0xF0) >> 4) *10;

		t_2 = (ptr[10] & 0x0F) + ((ptr[10] >> 4) * 10) + (ptr[11] & 0x0F)*100;
		t_2 -= 300;
		f_2 = (ptr[11] >> 4) + (ptr[12] & 0x0F)*10;
		printf(" Tin: %d T1: %d Fin: %d F1: %d T2: %d F2: %d  ", t_in, t_1, f_in, f_1, t_2, f_2);
		printf("\n");
/*		printf("  %02x  %02x %02x  %02x %02x \n",
			ptr[10], ptr[11],
			ptr[12], ptr[13], ptr[14]
			); */
	}

	return(0);
}
