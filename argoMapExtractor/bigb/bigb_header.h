#pragma once

#include <stdint.h>

struct BIGB_Header
{
	int32_t bigb;
	int32_t compressedDataLocation; // For I-Ninja, this happens to always be 384. At least with every LCM I've tried it on
	int32_t wadVersion;
	int32_t unk3;

	char name[0x40];
	char author[0x28];

	int32_t unk4;
	int32_t unk5;
	int32_t dataTableLocation; // This holds true for decompressed data
	int32_t dataTableSize; // This holds true for decompressed data
	int32_t unk8;
	int32_t unk9;

	char arguments[256];
};

static_assert(sizeof(BIGB_Header) == 400);

constexpr uint32_t BIGB_MAGIC = 0x42474942;
constexpr uint32_t BIGB_VERSION = 0x00000076;