#pragma once

struct VertexPNT
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct VertexPN
{
	float x, y, z;
	float nx, ny, nz;
};

static_assert(sizeof(VertexPNT) == 32, "Size must be 32 to match BIGB files");
static_assert(sizeof(VertexPN) == 24, "Size must be 24 to match BIGB files");
