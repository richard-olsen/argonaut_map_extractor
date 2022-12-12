#pragma once

#include <stdint.h>
#include <string>

constexpr uint32_t VOLT_HEADER_VALUE = 'TLOV';

struct VoltHeader
{
	uint32_t header = VOLT_HEADER_VALUE;
	uint32_t unk1 = 2;
	uint32_t fileCount = 0;
	uint32_t fileEntrySize = 0;
};

struct VoltDirectoryEntry
{
	uint32_t fileNameCRC = 0;
	uint32_t fileTypeID = 1;
	uint32_t fileEntryAccumulated; // Not sure of a better term for this value here
};

struct VoltFileEntry
{
	uint32_t fileOffset = 0;
	uint32_t unk1 = 0;
	uint32_t fileSize = 0;
	uint32_t unk2 = 0;
	std::string fileName;
};

constexpr uint32_t CORRECTED_VOLT_FILE_ENTRY_SIZE = 16;

static_assert(sizeof(VoltHeader) == 16, "Volt header");
static_assert(sizeof(VoltDirectoryEntry) == 12, "Directory Entry");
static_assert(sizeof(VoltFileEntry) == 16 + sizeof(std::string), "File Entry");

