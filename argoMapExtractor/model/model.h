#pragma once

#include <vector>
#include <string>

void WriteOBJVertexPNT(const std::vector<char> &decompressedData, size_t offset, const std::string &outputFileName);
void WriteOBJVertexPN(const std::vector<char> &decompressedData, size_t offset, const std::string &outputFileName);
