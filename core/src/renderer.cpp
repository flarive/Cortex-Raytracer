#include "renderer.h"



void renderer::render(scene& _scene, const renderParameters& _params, bool _multithreaded)
{
	
	_scene.get_camera()->initialize(_params);
	
	_scene.extract_lights();

	_scene.build_optimized_world();

	if (_multithreaded)
	{
		//const unsigned int CHUNKS_PER_THREAD = 4;

		//const unsigned int n_threads = std::thread::hardware_concurrency();
		//std::cout << "Detected " << n_threads << " concurrent threads." << std::endl;

		//render_multi_thread(_scene, _params, n_threads, CHUNKS_PER_THREAD);
	}
	else
	{
		render_single_thread(_scene, _params);
	}
}


void renderer::render_single_thread(scene& _scene, const renderParameters& _params)
{
	// write ppm file header
	//std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = 0; j < _scene.get_camera()->getImageHeight(); ++j)
	{
		
		
		
		if (!_params.quietMode)
			std::clog << "\rScanlines remaining: " << (_scene.get_camera()->getImageHeight() - j) << ' ' << std::flush;

		for (int i = 0; i < _scene.get_camera()->getImageWidth(); ++i)
		{
			// each pixel is black at the beginning
			color pixel_color(0, 0, 0);

			for (int s_j = 0; s_j < _scene.get_camera()->getSqrtSpp(); ++s_j)
			{
				for (int s_i = 0; s_i < _scene.get_camera()->getSqrtSpp(); ++s_i)
				{
					ray r = get_ray(i, j, s_i, s_j);

					// pixel color is progressively being refined
					pixel_color += ray_color(r, max_depth, _scene);
				}
			}

			// write ppm file color entry
			color::write_color(std::cout, i, j, pixel_color, samples_per_pixel);
		}
	}

	if (!_params.quietMode)
		std::clog << "\rDone.                 \n";
}
