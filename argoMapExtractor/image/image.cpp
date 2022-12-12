#include "image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../util/stb_image_write.h"
#include <data_manip.h>

struct BIGBImageEntry
{
	char entry[4];
	int width;
	int height;
	int unk1;
	uint32_t imageDataLocation;
	uint32_t imageDataSize;
	int unk2;
};

static_assert(sizeof(BIGBImageEntry) == 28, "Must be size of 28 for Image Entry");

#pragma pack(push)
#pragma pack(1)
struct ImageDetails
{
	char unk1;
	char hasColorTable;
	char dataType;
	char unk2;
	char unk3;
	short colorTableSize;
	short bitsPerColor;
	char padding[3];
	short width;
	short height;
	short bitsPerComponent;
};
#pragma pack(pop)

static_assert(sizeof(ImageDetails) == 18, "Image Data must be size of 18");

void WriteArgoImage(const std::vector<char> &decompressedData, size_t offset, const std::string &outputName)
{
	BIGBImageEntry *entry = (BIGBImageEntry *) &decompressedData[offset];
	ImageDetails *details = (ImageDetails *) &decompressedData[entry->imageDataLocation];
	uint32_t dataLength = entry->imageDataSize - sizeof(ImageDetails);

	int outImageSize = entry->width * entry->height * 4;
	std::vector<char> pixels;
	pixels.resize(outImageSize);

	if (details->hasColorTable)
	{
		int components = details->bitsPerColor / 8;
		int colorTableSize = details->colorTableSize *= components;
		size_t colorTableLocation = offset + 50;
		size_t imageIndexLocation = colorTableLocation + colorTableSize;

		for (int y = 0; y < entry->height; y++)
		{
			for (int x = 0; x < entry->width; x++)
			{
				size_t location = x + y * entry->width;
				unsigned int index = ((unsigned char) decompressedData[imageIndexLocation + location]);

				size_t imageLocation = (x + (entry->height - y - 1) * entry->width) * 4;
				size_t indexLocation = index * components;

				pixels[imageLocation + 0] = decompressedData[colorTableLocation + indexLocation + 2];
				pixels[imageLocation + 1] = decompressedData[colorTableLocation + indexLocation + 1];
				pixels[imageLocation + 2] = decompressedData[colorTableLocation + indexLocation + 0];
				pixels[imageLocation + 3] =
					components == 4 ?
						decompressedData[colorTableLocation + indexLocation + 3] :
						0xFF;
			}
		}
	}
	else
	{
		size_t imageOffset = offset + 50;
		int components = details->bitsPerComponent / 8;

		for (int y = 0; y < entry->height; y++)
		{
			for (int x = 0; x < entry->width; x++)
			{
				int imageLocation = (x + (entry->height - y - 1) * entry->width) * 4;
				int argoImageLocation = (x + y * entry->width) * components;

				pixels[imageLocation + 0] = decompressedData[imageOffset + argoImageLocation + 2];
				pixels[imageLocation + 1] = decompressedData[imageOffset + argoImageLocation + 1];
				pixels[imageLocation + 2] = decompressedData[imageOffset + argoImageLocation + 0];
				pixels[imageLocation + 3] =
					components == 4 ?
						decompressedData[imageOffset + argoImageLocation + 3] :
						0xFF;
			}
		}
	}

	stbi_write_png(outputName.c_str(), entry->width, entry->height, 4, &pixels[0], 0);
}

