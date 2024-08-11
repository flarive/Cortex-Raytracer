#include "renderer.h"


#include "../outputs/standard_output.h"

renderer::renderer(unsigned int nb_cores) : m_nb_core(nb_cores)
{
}

void renderer::render(scene& _scene, camera& _camera, const renderParameters& _params, std::shared_ptr<sampler> aa_sampler) const
{
}

void renderer::preview_line(const output& out, int j, std::vector<color> colors, int spp)
{
	for (unsigned int n = 0; n < colors.size(); n++)
	{
		color ff = color::prepare_pixel_color(n, j, colors[n], spp, true);

		int res = out.write_to_output(n, j, ff);
		if (res > 0)
		{
			std::cerr << "[ERROR] Error while writing to output" << std::endl;
		}
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
