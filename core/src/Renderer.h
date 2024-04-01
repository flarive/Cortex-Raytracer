#pragma once

#include <random>

#include "misc/scene.h"
#include "misc/ray.h"
#include "misc/color.h"
#include "utilities/types.h"
#include "misc/scatter_record.h"
#include "FloatImage.h"
#include "Random.h"




class Renderer
{
public:
	explicit Renderer(std::unique_ptr<scene> scene, Random::SeedType randomSeed = Random::GeneratorType::default_seed);

	int samplesPerPixels() const;

	int maxDepth() const;

	void setSamplesPerPixels(int samplesPerPixels);

	void setMaxDepth(int maxDepth);

	[[nodiscard]] FloatImage render(int width, int height);

	[[nodiscard]] FloatImage renderWithoutGammaCorrection(int width, int height);

	bool saveAsPPM(const FloatImage& image, const std::string& filename);

private:
	//[[nodiscard]] vector3 computeRayColor(const ray& ray, int depth, Random& randomGenerator) const;

	[[nodiscard]] color ray_color(const ray& r, int depth, Random& randomGenerator);

	int m_samplesPerPixels;
	int m_maxDepth;
	const double m_minT;

	color   background;
	
	Random::SeedType m_randomSeed;
	
	std::unique_ptr<scene> m_scene;

	int    sqrt_spp;        // Square root of number of samples per pixel
	double recip_sqrt_spp;  // 1 / sqrt_spp
};
