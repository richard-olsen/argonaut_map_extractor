#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

#include <crc32.h>
#include <volt.h>
#include <data_manip.h>

uint32_t g_crcTable[256];
constexpr size_t FILE_ALIGNMENT = 0x800;

int main(int argc, char *argv[])
{
	crc32::generate_table(g_crcTable);

	if (argc < 2)
	{
		std::cerr << "There MUST be a file added into the archive!" << std::endl;
		return 1;
	}

	VoltHeader header;

	std::vector<VoltDirectoryEntry> directoryTable;
	std::vector<VoltFileEntry> fileTable;

	int accumulatedFileEntrySize = 0;
	uint32_t crc = 0;

	for (int i = 0; i < (argc - 1); i++)
	{
		std::string fileName = argv[i + 1];
		VoltDirectoryEntry dEntry;
		VoltFileEntry fEntry;

		fEntry.fileSize = std::filesystem::file_size(fileName);
		fEntry.fileName = fileName.substr(fileName.find_last_of('\\') + 1);
		crc = crc32::update(g_crcTable, 0, fEntry.fileName.c_str(), fEntry.fileName.size());
		dEntry.fileNameCRC = crc;

		int len = fEntry.fileName.size() + 1;
		int fileHeader = CORRECTED_VOLT_FILE_ENTRY_SIZE;
		dEntry.fileEntryAccumulated = accumulatedFileEntrySize;
		accumulatedFileEntrySize += fileHeader + len;

		directoryTable.push_back(dEntry);
		fileTable.push_back(fEntry);
	}

	std::vector<std::vector<char>> outFiles;
	outFiles.resize(argc - 1);

	int offset = sizeof(VoltHeader) +
		(sizeof(VoltDirectoryEntry) * directoryTable.size()) +
		accumulatedFileEntrySize;

	header.fileEntrySize = accumulatedFileEntrySize;

	int fileSizesCombined = 0;
	int nextOffset = offset;

	for (int i = 0; i < outFiles.size(); i++)
	{
		std::ifstream file(argv[i + 1], std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "File can't be opened! {" << argv[i + 1] << "}" << std::endl;
			return 1;
		}
		fileSizesCombined += fileTable[i].fileSize;
		outFiles[i].resize(fileTable[i].fileSize);
		file.read(&(outFiles[i][0]), fileTable[i].fileSize);
		file.close();

		fileTable[i].fileOffset = nextOffset;
		nextOffset += fileTable[i].fileSize;
	}

	std::vector<char> outVolt;

	header.fileCount = fileTable.size();

	// Reverse tables to insert them into the outVolt file
	outVolt.insert(outVolt.end(), (char *)&header, (char *)&header + sizeof(VoltHeader));

	std::vector<int> fileOffsets; // Used to go back and reset the offsets

	for (int i = 0; i < directoryTable.size(); i++)
	{
		outVolt.insert(outVolt.end(), (char *)&directoryTable[i], (char *)&directoryTable[i] + sizeof(VoltDirectoryEntry));
	}
	for (int i = 0; i < fileTable.size(); i++)
	{
		int end = fileTable[i].fileName.size();

		fileOffsets.push_back(outVolt.size());

		outVolt.insert(outVolt.end(), (char *)&fileTable[i], (char *)&fileTable[i] + CORRECTED_VOLT_FILE_ENTRY_SIZE);
		outVolt.insert(outVolt.end(), fileTable[i].fileName.begin(), fileTable[i].fileName.end());
		outVolt.push_back(0); // Strings must be null terminated in the VOLT file
	}


	size_t filesEmplaced = 0;
	while (filesEmplaced < outFiles.size())
	{
		if ((outVolt.size() % FILE_ALIGNMENT) == 0)
		{
			fileTable[filesEmplaced].fileOffset = outVolt.size();

			outVolt.insert(outVolt.end(), outFiles[filesEmplaced].begin(), outFiles[filesEmplaced].end());

			filesEmplaced++;
		}
		else
		{
			outVolt.push_back(0x69);
		}
	}

	for (int i = 0; i < fileOffsets.size(); i++)
	{
		setIntoData(outVolt, fileOffsets[i], fileTable[i].fileOffset);
	}


	std::ofstream outFile("./outVolt.vol", std::ios::binary);
	outFile.write(&outVolt[0], outVolt.size());
	outFile.close();


	return 0;
}
