#include "renderer.h"

renderer::renderer()
{
}

void renderer::render(scene& _scene, camera& _camera, const renderParameters& _params, std::shared_ptr<sampler> aa_sampler) const
{
}

void renderer::preview_line(int j, std::vector<color> i, int spp)
{
	for (unsigned int n = 0; n < i.size(); n++)
	{
		color::write_color(std::cout, n, j, i[n], spp);
	}
}

bool renderer::saveToFile(string filepath, std::vector<std::vector<color>> image, int width, int height, int spp)
{
	// save image to disk
	uint8_t* data = bitmap_image::buildPNG(image, width, height, spp, true);
	if (data)
	{
		constexpr int CHANNELS = 3;
		return bitmap_image::saveAsPNG(filepath, width, height, CHANNELS, data, width * CHANNELS);
	}

	return false;
}
