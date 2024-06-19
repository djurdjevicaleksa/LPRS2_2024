

#pragma once

#include <stdint.h>


#define MAGIC 0xbabadeda

struct smpl_pkg
{
	uint32_t magic;
	uint32_t id;
	uint16_t val_array[6];
	uint8_t err;

} __attribute__((packed));


