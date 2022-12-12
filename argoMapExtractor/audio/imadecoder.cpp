#include "imadecoder.h"


int table[89] = {
	7, 8, 9, 10,
	11, 12, 13, 14,
	16, 17, 19, 21,
	23, 25, 28, 31,
	34, 37, 41, 45,
	50, 55, 60, 66,
	73, 80, 88, 97,
	107, 118, 130, 143,
	157, 173, 190, 209,
	230, 253, 279, 307,
	337, 371, 408, 449,
	494, 544, 598, 658,
	724, 796, 876, 963,
	1060, 1166, 1282, 1411,
	1552, 1707, 1878, 2066,
	2272, 2499, 2749, 3024,
	3327, 3660, 4026, 4428,
	4871, 5358, 5894, 6484,
	7132, 7845, 8630, 9493,
	10442, 11487, 12635, 13899,
	15289, 16818, 18500, 20350,
	22385, 24623, 27086, 29794,
	32767
};


short ima_step_table[89] = {
	 0x7, 0x8, 0x9, 0xA,
	 0xB, 0xC, 0xD, 0xE,
	0x10, 0x11, 0x13, 0x15,
	0x17, 0x19, 0x1C, 0x1F,
	0x22, 0x25, 0x29, 0x2D,
	0x32, 0x37, 0x3C, 0x42,
	0x49, 0x50, 0x58, 0x61,
	0x6B, 0x76, 0x82, 0x8F,
	0x9D, 0xAD, 0xBE, 0xD1,
	0xE6, 0xFD, 0x117, 0x133,
 0x151, 0x173, 0x198, 0x1C1,
 0x1EE, 0x220, 0x256, 0x292,
 0x2D4, 0x31C, 0x36C, 0x3C3,
 0x424, 0x48E, 0x502, 0x583,
 0x610, 0x6AB, 0x756, 0x812,
 0x8E0, 0x9C3, 0xABD, 0xBD0,
 0xCFF, 0xE4C, 0xFBA, 0x114C,
0x1307, 0x14EE, 0x1706, 0x1954,
0x1BDC, 0x1EA5, 0x21B6, 0x2515,
0x28CA, 0x2CDF, 0x315B, 0x364B,
0x3BB9, 0x41B2, 0x4844, 0x4F7E,
0x5771, 0x602F, 0x69CE, 0x7462,
0x7FFF
};

int ima_index_table[16] = {
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};

template<typename T>
inline void clip(T &out, T low, T high)
{
	out = (out > high) ?
		high :
		(out < low ? low : out);
}

void DecodeBlock(const std::vector<char> &block, std::vector<short> &out, int channel)
{
	std::vector<short> output;
	output.resize(block.size() * 2);
	int decoderPredicted[2];
	int decoderIndex[2];

	int index = 0;
	int samplesWritten = 0;

	for (int ch = 0; ch < channel; ch++)
	{
		decoderPredicted[ch] = *((short *) &block[index]);
		decoderIndex[ch] = block[index + 2];
		output[samplesWritten] = decoderPredicted[ch];

		index += 4;
		samplesWritten++;
	}

	while (index < block.size())
	{
		for (int ch = 0; ch < channel; ch++)
		{
			for (int i = 0; i < 4; i++)
			{
				int step = ima_step_table[decoderIndex[ch]];
				int delta = step >> 3;

				if (block[index] & 0x01) delta += (step >> 2);
				if (block[index] & 0x02) delta += (step >> 1);
				if (block[index] & 0x04) delta += step;
				if (block[index] & 0x08) delta = -delta;

				decoderPredicted[ch] += delta;
				decoderIndex[ch] += ima_index_table[(int) block[index] & 0x07];
				clip(decoderPredicted[ch], -32768, 32767);
				clip(decoderIndex[ch], 0, 88);
				output[samplesWritten + ch + (i * 2 * channel)] = decoderPredicted[ch];
				//output[samplesWritten + (ch * 2) + (i * 2 * channel)] = decoderPredicted[ch];

				step = ima_step_table[decoderIndex[ch]];
				delta = step >> 3;

				if (block[index] & 0x10) delta += (step >> 2);
				if (block[index] & 0x20) delta += (step >> 1);
				if (block[index] & 0x40) delta += step;
				if (block[index] & 0x80) delta = -delta;

				decoderPredicted[ch] += delta;
				decoderIndex[ch] += ima_index_table[((int) block[index] & 0x70) >> 4];
				clip(decoderPredicted[ch], -32768, 32767);
				clip(decoderIndex[ch], 0, 88);
				output[samplesWritten + ch + (i * 2 + 1) * channel] = decoderPredicted[ch];
				//output[samplesWritten + (ch * 2) + (i * 2 * channel) + 1] = decoderPredicted[ch];
				index++;
			}
		}
		samplesWritten += (2 * 4 * channel);
	}

	for (int i = 0; i < samplesWritten; i++)
	{
		out.push_back(output[i]);
	}
}

void Decode(const char *imaadpcm, int imasize, std::vector<short> &out, int blockAlign, int channel)
{
	std::vector<char> block; // size / block align times to fill this array
	block.resize(blockAlign);
	std::vector<short> decodedBlock;
	decodedBlock.resize(block.size() * 2);

	int blockCount = imasize / blockAlign;
	for (int i = 0; i < blockCount; i++)
	{
		for (int j = 0; j < blockAlign; j++)
		{
			int index = (i * blockAlign) + j;

			block[j] = imaadpcm[index];
		}

		//int compositeSamples = adpcm_decode_block(decodedBlock.data(), (unsigned char*)block.data(), block.size(), channel);
		//out.insert(out.end(), decodedBlock.begin(), decodedBlock.end());
		DecodeBlock(block, out, channel);
	}
}
