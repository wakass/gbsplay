/*
 * gbsplay is a Gameboy sound player
 *
 * 2015 (C) by Tobias Diedrich <ranma+gbsplay@tdiedrich.de>
 *
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#include "common.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "plugout.h"

static FILE *file;
static long cycles_prev = 0;

static long regparm iorawdumper_open(enum plugout_endian endian, long rate)
{
	/*
	 * clone and close STDOUT_FILENO
	 * to make sure nobody else can write to stdout
	 */
	int fd = dup(STDOUT_FILENO);
	if (fd == -1) return -1;
	(void)close(STDOUT_FILENO);
	file = fdopen(fd, "w");

	return 0;
}

static int regparm iorawdumper_skip(int subsong)
{
	return 0;
}

static int regparm iorawdumper_io(long cycles, uint32_t addr, uint8_t val)
{
	long cycle_diff = cycles - cycles_prev;

	if (addr >= 0xFF10 && addr <= 0xFF14) { //only get the registers we support	

		uint8_t addr_short = addr & 0xFF;
		uint32_t cycle_diff_short = (uint32_t) cycle_diff;
		fwrite(&cycle_diff_short, sizeof(uint32_t), 1, file);
		fwrite(&addr_short, sizeof(uint8_t), 1, file);
		fwrite(&val, sizeof(uint8_t), 1, file);
		cycles_prev = cycles; //only store for filtered
	}
	

	return 0;
}

static void regparm iorawdumper_close(void)
{
	fflush(file);
	fclose(file);
}

const struct output_plugin plugout_iorawdumper = {
	.name = "iorawdumper",
	.description = "STDOUT io dumper of raw",
	.open = iorawdumper_open,
	.skip = iorawdumper_skip,
	.io = iorawdumper_io,
	.close = iorawdumper_close,
	.flags = PLUGOUT_USES_STDOUT,
};
