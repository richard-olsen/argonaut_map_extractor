#pragma once

#include "bigb_header.h"

#include <vector>
#include <string>

void BIGBAllocateForCompressedData(const BIGB_Header &header, std::vector<char> &output);
void BIGBDecompress(const std::vector<char> &compressedData, std::vector<char> &decompressedData);
void BIGBOutputHeaderDetails(const BIGB_Header &header, const std::string &outputFile);
void BIGBExtractAllKnownData(const BIGB_Header &header, const std::vector<char> &decompressedData, const std::string &directory, std::ifstream &audioBank, uint32_t wavsOffset);
