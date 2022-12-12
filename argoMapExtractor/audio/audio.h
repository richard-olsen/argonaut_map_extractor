#pragma once

#include <vector>
#include <string>
#include <fstream>

void WriteArgoWav(const std::vector<char> &decompressedData, size_t offset, const std::string &audiosDirectory, std::ifstream &audioBank, uint32_t wavsOffset);
