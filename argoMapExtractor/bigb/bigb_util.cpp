#include "bigb_util.h"

#include <fstream>
#include <iostream>

#include <data_manip.h>

#include "../audio/audio.h"
#include "../model/model.h"
#include "../image/image.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void BIGBAllocateForCompressedData(const BIGB_Header &header, std::vector<char> &output)
{
	output.resize(header.unk8 + header.unk9);
}

void BIGBDecompress(const std::vector<char> &compressedData, std::vector<char> &decompressedData)
{
	size_t index = 0;
	size_t outIndex = 0;

	if (compressedData.empty())
		return;

	do
	{
		char current = compressedData[index];
		if (current < '\0')
		{
			index++;
			char peak = compressedData[index];
			index++;
			int count = (peak & 0xf) + 1;

			union
			{
				short cx;
				struct
				{
					char cl;
					char ch;
				};
			} thing = { 0 };

			thing.cl = peak;
			thing.ch = current;
			thing.cx = thing.cx >> 4;
			thing.cx += -1;
			int backtrack = decompressedData.size() + thing.cx;
			do
			{
				decompressedData.push_back(decompressedData[backtrack++]);
				count -= 1;
			} while (-1 < count);
		}
		else
		{
			if (current < '@')
			{
				index++;
				do
				{
					decompressedData.push_back(compressedData[index]);
					index++;
					current -= 1;
				} while (-1 < current);
			}
			else
			{
				// RLE Decoding
				index++;
				char rle = compressedData[index];
				current = current + -0x3e;
				index++;
				do
				{
					decompressedData.push_back(rle);
					current -= 1;
				} while ('\0' < current);
			}

		}
	} while (index < compressedData.size());
}

void BIGBOutputHeaderDetails(const BIGB_Header &header, const std::string &outputFile)
{
	std::ofstream out(outputFile);

	out.clear();

	out << "BIGB: " << header.bigb
	<< "\n\tCompressed Data Location - 0x16: " << header.compressedDataLocation
	<< "\n\tWAD Version: " << header.wadVersion
	<< "\n\tUnk3: " << header.unk3
	<< "\n\tName: " << header.name
	<< "\n\tAuthor: " << header.author
	<< "\n\tUnk4: " << header.unk4
	<< "\n\tUnk5: " << header.unk5
	<< "\n\tData Table Location: " << header.dataTableLocation
	<< "\n\tData Table Size: " << header.dataTableSize
	<< "\n\tUnk8: " << header.unk8
	<< "\n\tUnk9: " << header.unk9
	<< "\n\tArguments: " << header.arguments
	<< std::endl;

	out.close();
}

constexpr uint32_t BIGB_DATA_MARKER = 0x80000000;

constexpr uint32_t BIGB_DATA_WAV = 0x80000113;
constexpr uint32_t BIGB_DATA_IMAGE = 0x80000001;
constexpr uint32_t BIGB_DATA_MODEL = 0x80000024;
constexpr uint32_t BIGB_DATA_MODEL_2 = 0x80000023;

void BIGBExtractAllKnownData(const BIGB_Header &header, const std::vector<char> &decompressedData, const std::string &directory, std::ifstream &audioBank, uint32_t wavsOffset)
{
	size_t textureCount = 0;
	size_t audioCount = 0;
	size_t modelCount = 0;

	std::string imagesDirectory = directory + "images\\";
	std::string audiosDirectory = directory + "audios\\";
	std::string modelsDirectory = directory + "models\\";
	CreateDirectoryA(imagesDirectory.c_str(), NULL);
	CreateDirectoryA(audiosDirectory.c_str(), NULL);
	CreateDirectoryA(modelsDirectory.c_str(), NULL);

	size_t lastIndex = header.dataTableLocation + header.dataTableSize;
	if (lastIndex > decompressedData.size())
	{
		std::cout << "Warning, there might be some missing data" << std::endl;
		lastIndex = decompressedData.size();
	}
	for (size_t i = header.dataTableLocation; i < lastIndex; i += 4)
	{
		uint32_t currentValue = getFromData<uint32_t>(decompressedData, i);

		if (currentValue == BIGB_DATA_IMAGE)
		{
			i += 4;
			uint32_t location = getFromData<uint32_t>(decompressedData, i);
			textureCount++;
			WriteArgoImage(decompressedData, location, imagesDirectory + std::to_string(textureCount) + ".png");

			// Generates a lot of images, but are messed up. Though it does look like it might be for texture animations
			/*uint32_t trigger = getFromData<uint32_t>(decompressedData, i + 4);
			while ((trigger & BIGB_DATA_MARKER) == 0)
			{
				i += 4;
				uint32_t size = getFromData<uint32_t>(decompressedData, trigger - 4);

				for (uint32_t j = 0; j < size; j++)
				{
					i += 4;
					uint32_t location2 = getFromData<uint32_t>(decompressedData, i);
					uint32_t textureLocation = getFromData<uint32_t>(decompressedData, location2);

					textureCount++;
					WriteArgoImage(decompressedData, textureLocation, imagesDirectory + std::to_string(textureCount) + "_.png");
				}

				trigger = getFromData<uint32_t>(decompressedData, i + 4);
			}*/
		}
		else if (currentValue == BIGB_DATA_WAV)
		{
			i += 4;
			uint32_t location = getFromData<uint32_t>(decompressedData, i);
			audioCount++;
			WriteArgoWav(decompressedData, location, audiosDirectory, audioBank, wavsOffset);
		}
		else if (currentValue == BIGB_DATA_MODEL || currentValue == BIGB_DATA_MODEL_2)
		{
			i += 4;
			uint32_t location = getFromData<uint32_t>(decompressedData, i);
			modelCount++;
			std::string str = std::to_string(modelCount);
			if (currentValue == BIGB_DATA_MODEL_2)
			{
				str += "_";
				WriteOBJVertexPN(decompressedData, location, modelsDirectory + str + ".obj");
			}
			else
			{
				WriteOBJVertexPNT(decompressedData, location, modelsDirectory + str + ".obj");
			}
		}
	}
}

