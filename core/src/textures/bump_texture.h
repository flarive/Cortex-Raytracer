#pragma once

#include "texture.h"
#include "../misc/color.h"

//static constexpr double rescale = 1.f / 255.f;

/// <summary>
/// Bump texture (NOT TESTED !!!!!!!!!)
/// </summary>
class bump_texture : public texture
{
public:
	bump_texture() {}
	
	bump_texture(unsigned char* pixels, int A, int B, int nn, double intensity, double repeatu = 1.f, double repeatv = 1.f) 
		: data(pixels), nx(A), ny(B), channels(nn), intensity(intensity), repeatu(repeatu), repeatv(repeatv) {}


	color value(double u, double v, const point3& p) const
	{
		while (u < 0) u += 1;
		while (v < 0) v += 1;
		while (u > 1) u -= 1;
		while (v > 1) v -= 1;
		u = fmod(u * repeatu, 1);
		v = fmod(v * repeatv, 1);
		int i = u * (nx - 1);
		int j = (1 - v) * (ny - 1);
		if (i < 1) i = 1;
		if (j < 1) j = 1;
		if (i > nx - 2) i = nx - 2;
		if (j > ny - 2) j = ny - 2;
		double bu = double(data[channels * (i + 1) + channels * nx * j] - data[channels * (i - 1) + channels * nx * j]) / 2 * rescale;
		double bv = double(data[channels * i + channels * nx * (j + 1)] - data[channels * i + channels * nx * (j - 1)]) / 2 * rescale;
		return(color(intensity * bu, intensity * bv, 0));
	}

private:
	unsigned char* data;
	int nx, ny, channels;
	double intensity;
	double repeatu, repeatv;
};