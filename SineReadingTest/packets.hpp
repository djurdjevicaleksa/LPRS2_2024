

#pragma once

#include <stdint.h>


#define MAGIC 0xbabadeda

struct smpl_pkg
{
	uint32_t magic;
	uint32_t id;

	struct {

		uint8_t timestamp[5];
		uint16_t val_array[6];
		uint8_t err;
	} data;

} __attribute__((packed));


