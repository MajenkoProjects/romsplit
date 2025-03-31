#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgen.h>
#include <string.h>

struct options {
	const char *combined;
	const char *split[4];
	int help;
	int unknown;
};

void get_options(int argc, char **argv, struct options *opts) {

	int opt;
	
	while((opt = getopt(argc, argv, "1:2:3:4:c:h")) > 0) {
		switch (opt) {
			case 'h': opts->help++; break;
			case '1': opts->split[0] = optarg; break;
			case '2': opts->split[1] = optarg; break;
			case '3': opts->split[2] = optarg; break;
			case '4': opts->split[3] = optarg; break;
			default: opts->unknown++; break;
		}
	}
}

void rs_usage() {
	printf("Usage: romsplit -c input.rom -1 byte1.rom -2 byte2.rom\n");
	printf("                -3 byte3.rom -4 byte4.rom\n");
	printf("Splits a 32-bit ROM file into four individual 8-bit ROM\n");
	printf("files ready for writing on to 8-bit EPROM chips.\n");
}

void rj_usage() {
	printf("Usage: romjoin-c input.rom -1 byte1.rom -2 byte2.rom\n");
	printf("              -3 byte3.rom -4 byte4.rom\n");
	printf("Joins four individual 8-bit ROM files into a single\n");
	printf("32-bit ROM file in the order 1234 ready for emulation\n");
	printf("or disassembly.\n");
}


int romsplit(int argc, char **argv) {

	struct options opts = {0};

	get_options(argc, argv, &opts);

	if (opts.help > 0) {
		rs_usage();
		return 0;
	}

	if ((opts.unknown > 0) || (opts.combined==0) || 
		(opts.split[0] == 0) || (opts.split[1] == 0) || 
		(opts.split[2] == 0) || (opts.split[3] == 0)) {

		rs_usage();
		return -1;
	}

	int fd = open(opts.combined, O_RDONLY);

	int b1 = open(opts.split[0], O_RDWR | O_CREAT | O_TRUNC, 0644);
	int b2 = open(opts.split[1], O_RDWR | O_CREAT | O_TRUNC, 0644);
	int b3 = open(opts.split[2], O_RDWR | O_CREAT | O_TRUNC, 0644);
	int b4 = open(opts.split[3], O_RDWR | O_CREAT | O_TRUNC, 0644);

	uint8_t data[4];

	while (read(fd, data, 4) == 4) {
		write(b1, &data[0], 1);
		write(b2, &data[1], 1);
		write(b3, &data[2], 1);
		write(b4, &data[3], 1);
	}
	close(fd);
	close(b1);
	close(b2);
	close(b3);
	close(b4);
	return 0;
}

int romjoin(int argc, char **argv) {
	struct options opts = {0};

	get_options(argc, argv, &opts);

	if (opts.help > 0) {
		rj_usage();
		return 0;
	}

	if ((opts.unknown > 0) || (opts.combined==0) || 
		(opts.split[0] == 0) || (opts.split[1] == 0) || 
		(opts.split[2] == 0) || (opts.split[3] == 0)) {

		rj_usage();
		return -1;
	}

	int fd = open(opts.combined, O_RDWR | O_CREAT | O_TRUNC, 0644);

	int b1 = open(opts.split[0], O_RDONLY);
	int b2 = open(opts.split[1], O_RDONLY);
	int b3 = open(opts.split[2], O_RDONLY);
	int b4 = open(opts.split[3], O_RDONLY);

	uint8_t data[4];

	while (read(b1, &data[0], 1) == 1) {
		read(b2, &data[1], 1);
		read(b3, &data[2], 1);
		read(b4, &data[3], 1);
		write(fd, data, 4);
	}
	close(fd);
	close(b1);
	close(b2);
	close(b3);
	close(b4);
	return 0;
}

int main(int argc, char **argv) {
	const char *progname = basename(argv[0]);

	if (strcmp(progname, "romsplit") == 0) return romsplit(argc, argv);
	if (strcmp(progname, "romjoin") == 0) return romjoin(argc, argv);
	return -1;
}
