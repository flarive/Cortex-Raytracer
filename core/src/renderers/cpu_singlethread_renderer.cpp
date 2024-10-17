#include "cpu_singlethread_renderer.h"

#include "../outputs/no_output.h"
#include "../outputs/standard_output.h"
#include "../outputs/namedpipes_output.h"



cpu_singlethread_renderer::cpu_singlethread_renderer(unsigned int nb_cores) : renderer(nb_cores)
{
}

void cpu_singlethread_renderer::render(scene& _scene, camera& _camera, const renderParameters& _params, std::shared_ptr<sampler> aa_sampler) const
{
	int image_height = _camera.getImageHeight();
	int image_width = _camera.getImageWidth();
	int sqrt_spp = _camera.getSqrtSpp();
	int max_depth = _camera.getMaxDepth();
	int spp = _camera.getSamplePerPixel();

	std::cout << "[INFO] Starting single thread rendering" << std::endl;

	std::vector<std::vector<color>> image(image_height, std::vector<color>(image_width, color()));

	std::unique_ptr<output> out;


	if (_params.quietMode)
	{
		out = std::make_unique<namedpipes_output>();
	}
	else
	{
		out = std::make_unique<no_output>();
	}

	out->init_output(24);


	for (int j = 0; j < image_height; ++j)
	{
		if (!_params.quietMode)
			std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

		for (int i = 0; i < image_width; ++i)
		{
			// each pixel is black at the beginning
			color pixel_color(0, 0, 0);

			for (int s_j = 0; s_j < sqrt_spp; ++s_j)
			{
				for (int s_i = 0; s_i < sqrt_spp; ++s_i)
				{
					ray r = _camera.get_ray(i, j, s_i, s_j, aa_sampler);

					// pixel color is progressively being refined
					pixel_color += _camera.ray_color(r, max_depth, _scene);

					image[j][i] = pixel_color;
				}
			}

			// output
			out->write_to_output(i, j, color::prepare_pixel_color(i, j, pixel_color, spp, _params.useGammaCorrection));
		}
	}

	if (_params.quietMode)
		std::cout << std::endl << "[INFO] Rendering completed !" << std::endl;

	std::cout << std::flush;

	if (!_params.quietMode)
		std::clog << "\r[INFO] Done.                 \n";


	out->clean_output();


	// save to disk if needed
	if (!_params.saveFilePath.empty())
	{
		if (saveToFile(_params.saveFilePath, image, image_width, image_height, spp))
		{
			std::clog << "\r[INFO] Image saved to " << _params.saveFilePath << "\n";
		}
	}
}