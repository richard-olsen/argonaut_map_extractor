#include "model.h"
#include "vertex.h"

#include <stdint.h>
#include <fstream>

struct BIGB_ModelEntry
{
	char entry[4];
	uint32_t unk1;
	uint32_t unk2;
	uint32_t vertexCount; // Element Count, multiply by 32 to get total bytes
	uint32_t vertexDataLocation;
	uint32_t indexCount; // Element Count, multiply by 2 to get total bytes
	uint32_t indexDataLocation;
};

void outputFormat(int index, std::ofstream &output);

void WriteOBJVertexPNT(const std::vector<char> &decompressedData, size_t offset, const std::string &outputFileName)
{
	BIGB_ModelEntry *entry = (BIGB_ModelEntry *) &decompressedData[offset];

	const VertexPNT *vertices = (const VertexPNT *)&decompressedData[entry->vertexDataLocation];
	const unsigned short *indices = (const unsigned short *)&decompressedData[entry->indexDataLocation];

	std::ofstream outputFile;
	outputFile.open(outputFileName, std::ios_base::binary);
	outputFile.clear();

	for (int i = 0; i < entry->vertexCount; i++)
	{
		const VertexPNT &vertex = vertices[i];
		outputFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
	}
	for (int i = 0; i < entry->vertexCount; i++)
	{
		const VertexPNT &vertex = vertices[i];
		outputFile << "vt " << vertex.u << " " << 1.0f - vertex.v << "\n";
	}
	for (int i = 0; i < entry->vertexCount; i++)
	{
		const VertexPNT &vertex = vertices[i];
		outputFile << "vn " << vertex.nx << " " << vertex.ny << " " << vertex.nz << "\n";
	}

	// Treat it like a triangle strip
	for (int i = 0; i < entry->indexCount - 2; i++)
	{
		int i1 = indices[i] + 1;
		int i2 = indices[i + 1] + 1;
		int i3 = indices[i + 2] + 1;

		if (i % 2 == 0)
			std::swap(i1, i2);

		outputFile << "f ";
		outputFormat(i1, outputFile);
		outputFile << " ";
		outputFormat(i2, outputFile);
		outputFile << " ";
		outputFormat(i3, outputFile);
		outputFile << "\n";
	}
	outputFile.close();
}

void WriteOBJVertexPN(const std::vector<char> &decompressedData, size_t offset, const std::string &outputFileName)
{
	BIGB_ModelEntry *entry = (BIGB_ModelEntry *) &decompressedData[offset];

	const VertexPN *vertices = (const VertexPN *) &decompressedData[entry->vertexDataLocation];
	const unsigned short *indices = (const unsigned short *) &decompressedData[entry->indexDataLocation];

	std::ofstream outputFile;
	outputFile.open(outputFileName, std::ios_base::binary);
	outputFile.clear();

	for (int i = 0; i < entry->vertexCount; i++)
	{
		const VertexPN &vertex = vertices[i];
		outputFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
	}
	/*for (int i = 0; i < vertices.size(); i++)
	{
		const Vertex &vertex = vertices[i];
		outputFile << "vn " << vertex.nx << " " << vertex.ny << " " << vertex.nz << "\n";
	}*/

	// Treat it like a triangle strip
	for (int i = 0; i < entry->indexCount - 2; i++)
	{
		int i1 = indices[i] + 1;
		int i2 = indices[i + 1] + 1;
		int i3 = indices[i + 2] + 1;

		if (i % 2 == 0)
			std::swap(i1, i2);


		outputFile << "f " << i1 << " " << i2 << " " << i3 << "\n";
	}
	outputFile.close();

	/*std::ofstream outputRaw;
	outputRaw.open(outputFileName + ".damf", std::ios_base::binary);
	outputRaw.clear();

	struct
	{
		int bytesPerVertex;
		int vertexCount;
		int indexCount;
	} entryWay;
	entryWay.bytesPerVertex = entry->unk2;
	entryWay.vertexCount = entry->vertexCount;
	entryWay.indexCount = entry->indexCount;
	outputRaw.write((char *) &entryWay, sizeof(entryWay));
	outputRaw.write((char *) vertices, sizeof(VertexPN) * entry->vertexCount);
	outputRaw.write((char *) indices, sizeof(short) * entry->indexCount);
	outputRaw.close();*/
}

void outputFormat(int index, std::ofstream &output)
{
	char formatted[64] = { 0 };
	sprintf_s<64>(formatted, "%i/%i/%i", index, index, index);
	output << formatted;
}
