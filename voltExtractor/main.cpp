#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <Windows.h>

#include <stdint.h>

#include <volt.h>

void PrintVolt(VoltHeader &header, std::vector<VoltFileEntry> &files, std::vector<VoltDirectoryEntry> &dirs)
{
	std::cout << "Header Value: " << header.header << std::endl;
	std::cout << "Unknown: " << header.unk1 << std::endl;
	std::cout << "File Count: " << header.fileCount << std::endl;
	std::cout << "File Entry Size: " << header.fileEntrySize << std::endl;
	
	std::cout << "|--- " << dirs.size() << " Dirs" << std::endl;
	for (int i = 0; i < dirs.size(); i++)
	{
		std::cout << "|\tFile Name CRC: " << dirs[i].fileNameCRC << " | File Type ID: " << dirs[i].fileTypeID << " | FileEntry Accumulated: " << dirs[i].fileEntryAccumulated << std::endl;
	}
	std::cout << "|===\n\n|--- " << files.size() << " Files" << std::endl;
	for (int i = 0; i < files.size(); i++)
	{
		std::cout << "|\tFile Begin Offset: " << files[i].fileOffset << " | " << files[i].unk1 << std::endl;
		std::cout << "|\tFile Size: " << files[i].fileSize << " | " << files[i].unk2 << std::endl;
		std::cout << "|\tFile Name: " << files[i].fileName << std::endl;
	}
	std::cout << "|===" << std::endl;
}

int ReadVoltHeader(std::ifstream &stream, VoltHeader &header)
{
	stream.read((char*)&header, sizeof(header));
	
	return 0;
}

void ReadDirectoryTable(std::ifstream &stream, std::vector<VoltDirectoryEntry> &dirs)
{
	for (int i = 0; i < dirs.size(); i++)
	{
		stream.read((char*)&dirs[i], sizeof(VoltDirectoryEntry));
	}
}

void ReadFileTable(std::ifstream &stream, std::vector<VoltFileEntry> &files)
{
	for (int i = 0; i < files.size(); i++)
	{
		VoltFileEntry &file = files[i];
		
		stream.read((char*)&file, CORRECTED_VOLT_FILE_ENTRY_SIZE); // Read the next 16 bytes, then deal with the string
		
		char c;
		do
		{
			stream.read(&c, 1);
			if (c != 0)
				file.fileName.push_back(c);
		}while (c != 0);
	}
}

int ReadVolt(std::ifstream &stream, VoltHeader &header, std::vector<VoltFileEntry> &files, std::vector<VoltDirectoryEntry> &dirs)
{
	ReadVoltHeader(stream, header);
	
	// TODO Check if valid
	
	files.resize(header.fileCount);
	dirs.resize(header.fileCount);
	
	ReadDirectoryTable(stream, dirs);
	ReadFileTable(stream, files);
	
	return 0;
}

void WriteFiles(std::ifstream &stream, std::string &dir, std::vector<VoltFileEntry> &files)
{
	for (int i = 0; i < files.size(); i++)
	{
		std::ofstream out(dir + files[i].fileName, std::ios::binary);
		if (!out.is_open())
			continue;
		
		out.clear();
		
		stream.seekg(files[i].fileOffset);
		if (files[i].fileSize < 0)
		{
			out.close();
			continue;
		}
		char *file = new char[files[i].fileSize];
		stream.read(file, files[i].fileSize);
		
		out.write(file, files[i].fileSize);
		delete[] file;
		
		
		out.close();
	}
}

void ExtractVolt(const char *file)
{
	std::string fileName = file; // For easy manipulation of file

	std::ifstream volt = std::ifstream(fileName, std::ios::binary);
	if (!volt.is_open())
		return;
	
	VoltHeader header;
	std::vector<VoltDirectoryEntry> dirs;
	std::vector<VoltFileEntry> files;
	
	int ret = ReadVolt(volt, header, files, dirs);
	if (ret == -1) // -1 means the volt file is invalid
		return;
		
	std::string dir = fileName.substr(0, fileName.size() - 4) + "/"; // Get rid of the .vol or .avl extension
	std::string ext = fileName.substr(dir.size(), 3); // Get the vol file extension

	// Put all contents inside a subfolder for audio, only if the extension is .avl
	std::transform(ext.begin(), ext.end(), ext.begin(), std::tolower);
	if (ext == "avl")
	{
		CreateDirectoryA(dir.c_str(), 0); // Create first directory
		dir += "audio/";
	}
	
	CreateDirectoryA(dir.c_str(), 0); // Create directory named after the volt file
	WriteFiles(volt, dir, files);
		
	//PrintVolt(header, files, dirs);
	
	volt.close();
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return 1;
	for (int i = 1; i < argc; i++)
		ExtractVolt(argv[i]);
	
	return 0;
}