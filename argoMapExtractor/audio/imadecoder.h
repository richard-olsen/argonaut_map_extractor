#pragma once

#include <vector>


//void Decode(const char *imaadpcm, int imasize, std::vector<short> &out, int blockAlign = 72, int channel = 2)
void Decode(const char *imaadpcm, int imasize, std::vector<short> &out, int blockAlign = 36, int channel = 1);
