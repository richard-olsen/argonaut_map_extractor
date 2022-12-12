#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#if defined(_WIN64) || defined(WIN64)
#error Compile under 32 bit mode, errors will arise in 64 bit mode due to the size of some data types
#endif

#include "bigb/bigb_header.h"
#include "bigb/bigb_util.h"

int extractAllFrom(const std::string &fileName);
std::string getDirectoryFromFileName(const std::string &fileName);
std::string getFileExtension(const std::string &fileName);
std::string getIsolatedFileName(const std::string &fileName);

int main(int argc, char *argv[])
{
	if (argc <= 1)
	{
		std::cerr << "Needs at least 1 input of a .LCS or .LCM file.\nThe tool will then read through and attempt to extract all the contents from the files while also giving the decompressed data for further analysis" << std::endl;
	}

	MessageBox(NULL, "Debug", "", MB_ICONINFORMATION);

	for (int i = 1; i < argc; i++)
	{
		std::string file = argv[i];
		int result = extractAllFrom(file);

		std::cout << file << " - ";
		switch (result)
		{
		case 1:
			std::cout << "Failed to Read File";
			break;
		case 2:
			std::cout << "Incorrect File Type. Invalid BIGB File, or Unsupported Version";
			break;
		case 3:
			std::cout << "Failed to write Decompressed Data";
			break;
		case 4:
			std::cout << "Failed to open Audio Bank";
			break;
		default:
			std::cout << "Success";
			break;
		}
		std::cout << std::endl;
	}

	return 0;
}

int extractAllFrom(const std::string &fileName)
{
	std::ifstream input;
	input.open(fileName, std::ios_base::binary);
	if (!input.is_open())
	{
		return 1;
	}

	BIGB_Header header = { 0 };
	input.read((char *) &header, sizeof(BIGB_Header));

	if (header.bigb != BIGB_MAGIC || header.wadVersion != BIGB_VERSION)
		return 2;

	std::string extension = getFileExtension(fileName);
	std::string directory = getDirectoryFromFileName(fileName);
	std::string audioFileName = directory + "audio\\" + getIsolatedFileName(fileName) + ".PCA";
	directory += getIsolatedFileName(fileName);
	CreateDirectory(directory.c_str(), NULL);
	directory += ("\\" + extension + "\\");
	CreateDirectory(directory.c_str(), NULL);

	BIGBOutputHeaderDetails(header, directory + getIsolatedFileName(fileName) + "_details.txt");

	std::vector<char> compressedData;
	input.seekg(0x10 + header.compressedDataLocation);
	BIGBAllocateForCompressedData(header, compressedData);

	input.read(compressedData.data(), compressedData.size());
	input.close();

	std::vector<char> decompressedData;
	BIGBDecompress(compressedData, decompressedData);

	std::ofstream output;
	output.open(directory + getIsolatedFileName(fileName) + ".dat", std::ios_base::binary);
	if (!output.is_open())
		return 3;

	uint32_t wavsOffset = 0;
	if (header.unk4 > 0)
	{
		if ((header.unk4 & 0x00FFF) < 0x00800)
		{
			wavsOffset = (header.unk4 & 0xFF000) + 0x00800;
		}
		else
		{
			wavsOffset = (header.unk4 & 0xFF000) + 0x01000;
		}
	}

	output.clear();
	output.write(decompressedData.data(), decompressedData.size());
	output.close();

	std::ifstream audioBank;
	audioBank.open(audioFileName, std::ios_base::binary);
	if (!audioBank.is_open())
		return 4;
	
	BIGBExtractAllKnownData(header, decompressedData, directory, audioBank, wavsOffset);
	
	audioBank.close();

	return 0;
}

std::string getDirectoryFromFileName(const std::string &fileName)
{
	return fileName.substr(0, fileName.find_last_of('\\')) + '\\';
}

std::string getFileExtension(const std::string &fileName)
{
	return fileName.substr(fileName.find_last_of('.') + 1);
}

std::string getIsolatedFileName(const std::string &fileName)
{
	std::string str = fileName.substr(fileName.find_last_of('\\') + 1);
	return str.substr(0, str.find_last_of('.'));
}
