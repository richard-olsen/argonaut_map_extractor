#include "audio.h"

#include <data_manip.h>
#include <iostream>

#include "imadecoder.h"

constexpr int RIFF_MAGIC = 'FFIR';
constexpr int WAVE_FORMAT = 'EVAW';
constexpr int WAVS_FORMAT = 'SVAW';

struct RIFFheader
{
	int id = RIFF_MAGIC;
	int chunkSize = 0;
	int format = WAVE_FORMAT;
};

struct RIFFChunk
{
	int id;
	int size;
};

struct RIFFfmt
{
	short audioFormat = 0;
	short numChannels = 0;
	int sampleRate = 0;
	int byteRate = 0;
	short blockAlign = 0;
	short bitsPerSample = 0;
	short extraFormatBytes = 0; // Should be two
	short unknownData = 0;
};
struct RIFFfact
{
	union
	{
		int unk;
		struct
		{
			short smolUnk1;
			short smolUnk2;
		};
	};
};
struct RIFFwsmp
{
	int dlsSampleSize = 20;
	short unityNote = 0;
	short findTune = 0;
	int gain = 0;
	int options = 0;
	int loopCount = 0;
	int dlsLoopSize = 16;
	int loopType = 0;
	int loopStart = 0;
	int loopLength = 0;
};
struct RIFFstrm
{
	int fileOffset;
	int byteSize;
};
static_assert(sizeof(RIFFfact) == 4, "Size must be 4");
static_assert(sizeof(RIFFstrm) == 8, "Size must be 8");
static_assert(sizeof(RIFFwsmp) == 36, "Size must be 36");
static_assert(sizeof(RIFFfmt) == 20, "Size must be 20");

void WriteArgoWav(const std::vector<char> &decompressedData, size_t offset, const std::string &audiosDirectory, std::ifstream &audioBank, uint32_t wavsOffset)
{
	uint32_t someRandomOffset = 0;
	int waveCompare = getFromData<int>(decompressedData, offset + 8);
	if (waveCompare == WAVE_FORMAT)
	{
		someRandomOffset = 0;
	}
	else if (waveCompare == WAVS_FORMAT)
	{
		// 296960 is a magic value that works with robot beach (10-00.LCS), hopefully it's not so magical
		//someRandomOffset = 296960;
		someRandomOffset = 0x48800; // Works with Robot Beach Hub (10-00.LCS)
		//someRandomOffset = 0x38000;
		someRandomOffset = 0x48000; // Works with Mountain Gorge (40-00.LCS)
		someRandomOffset = 0x53000; // Works with Bomb Bay (20-00.LCS)
		someRandomOffset = 0x51000; // Works with Jungle Falls (30-00.LCS)
		someRandomOffset = 0x47000; // Works with Moon Base (50-00.LCS)
		someRandomOffset = wavsOffset;
		// Differences
		// -> 1868 Robot Beach
		// -> 1548 Mountain Gorge
		// -> 1856 Bomb Bay

		// Wonder if some bitwise operation can help with this
		// Need to transform, for example, 0x45C80 -> 0x46000
		// Or 0x50020 -> 0x50800?

	}
	else
	{
		std::cout << "Unknown Format " << std::string(&decompressedData[0], 4) << std::endl;
	}

	size_t size = getFromData<size_t>(decompressedData, offset + 0x04) + offset + 8;

	RIFFfmt *fmt = 0;

	RIFFfact *fact = 0;
	RIFFstrm *strm = 0;
	RIFFwsmp *wsmp = 0;

	bool hasWsmp = false;
	bool hasStrm = false;

	std::string fileName = "";

	size_t i = offset + 12;
	while (i < size)
	{
		uint32_t chunkID = getFromData<size_t>(decompressedData, i);
		uint32_t next = getFromData<uint32_t>(decompressedData, i + 4);
		i += 8;

		if (chunkID == 'eman')
		{
			fileName = std::string(&decompressedData[i]);
		}
		else if (chunkID == ' tmf')
		{
			fmt = (RIFFfmt *) &decompressedData[i];
		}
		else if (chunkID == 'tcaf')
		{
			fact = (RIFFfact *) &decompressedData[i];
		}
		else if (chunkID == 'pmsw')
		{
			wsmp = (RIFFwsmp *) &decompressedData[i];
		}
		else if (chunkID == 'mrts')
		{
			strm = (RIFFstrm *) &decompressedData[i];
		}

		i += next;
	}

	if (fmt == 0)
	{
		std::cerr << "No FMT chunk in wav file" << std::endl;
		return;
	}
	if (fact == 0)
	{
		std::cerr << "No FACT chunk in wav file" << std::endl;
		return;
	}
	if (strm == 0)
	{
		std::cerr << "No STRM chunk in wav file" << std::endl;
		return;
	}

	uint32_t audioOffset = strm->fileOffset + someRandomOffset;
	audioBank.seekg(audioOffset);
	std::vector<char> audioData;
	audioData.resize(strm->byteSize);
	audioBank.read(audioData.data(), strm->byteSize);

	std::vector<short> outputPCM;
	Decode(audioData.data(), strm->byteSize, outputPCM, fmt->blockAlign, fmt->numChannels);


	std::ofstream outputFile;
	outputFile.open(audiosDirectory + fileName, std::ios_base::binary);
	
	RIFFChunk chunk = { 0 };
	RIFFheader outHeader;
	RIFFfmt outFmt;
	outFmt.audioFormat = 1;
	outFmt.numChannels = fmt->numChannels;
	outFmt.sampleRate = fmt->sampleRate;
	outFmt.bitsPerSample = 16;
	outFmt.byteRate = (outFmt.sampleRate * outFmt.numChannels * outFmt.bitsPerSample) / 8;
	outFmt.blockAlign = (outFmt.numChannels * outFmt.bitsPerSample) / 8;

	outHeader.chunkSize = 4 + (8 + 16) + (8 + (outputPCM.size() * 2));

	outputFile.write((char *) &outHeader, sizeof(RIFFheader));
	chunk.id = ' tmf';
	chunk.size = 16;
	outputFile.write((char *) &chunk, sizeof(RIFFChunk));
	outputFile.write((char *) &outFmt, 16);
	chunk.id = 'atad';
	chunk.size = outputPCM.size() * 2;
	outputFile.write((char *) &chunk, sizeof(RIFFChunk));
	outputFile.write((char *) outputPCM.data(), outputPCM.size() * 2);
	outputFile.close();
}

