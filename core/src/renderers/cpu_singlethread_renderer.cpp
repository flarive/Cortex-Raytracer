#include "cpu_singlethread_renderer.h"

//#include "../randomizers/randomizer.h"

void cpu_singlethread_renderer::render(scene& _scene, camera& _camera, const renderParameters& _params, std::shared_ptr<sampler> aa_sampler) const
{
	int image_height = _camera.getImageHeight();
	int image_width = _camera.getImageWidth();
	int sqrt_spp = _camera.getSqrtSpp();
	int max_depth = _camera.getMaxDepth();
	int spp = _camera.getSamplePerPixel();

	std::cout << "[INFO] Starting single thread rendering" << std::endl;

	std::vector<std::vector<color>> image(image_height, std::vector<color>(image_width, color()));

	//randomizer* rnd = new randomizer(236544);

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

			// write ppm file color entry
			color::write_color(std::cout, i, j, pixel_color, spp);
		}
	}

	std::cout << "[INFO] Rendering completed !" << std::endl;

	std::cout << std::flush;

	if (!_params.quietMode)
		std::clog << "\rDone.                 \n";


	// save to disk if needed
	if (!_params.saveFilePath.empty())
	{
		if (saveToFile(_params.saveFilePath, image, image_width, image_height, spp))
		{
			std::cout << "[INFO] Saving image as PNG !" << std::endl;

			if (!_params.quietMode)
				std::clog << "\rImage saved to " << _params.saveFilePath << "\n";
		}
	}
}

