#pragma once

#include "texture.h"
#include "../misc/color.h"



/// <summary>
/// Bump texture (NOT TESTED !!!!!!!!!)
/// </summary>
class bump_texture : public texture
{
public:
	bump_texture();
	bump_texture(unsigned char* pixels, int A, int B, int nn, double intensity, double repeatu = 1.f, double repeatv = 1.f);

	color value(double u, double v, const point3& p) const;


private:
	unsigned char* data;
	int nx, ny, channels;
	double intensity;
	double repeatu, repeatv;
};