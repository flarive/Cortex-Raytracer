#include "cpu_multithread_renderer.h"

#include "../outputs/no_output.h"
#include "../outputs/standard_output.h"
#include "../outputs/namedpipes_output.h"

#include <thread>

cpu_multithread_renderer::cpu_multithread_renderer(unsigned int nb_cores) : renderer(nb_cores)
{
}

/// <summary>
/// https://github.com/Drummersbrother/raytracing-in-one-weekend
/// </summary>
/// <param name="_scene"></param>
/// <param name="_camera"></param>
/// <param name="_params"></param>
/// <param name="aa_sampler"></param>
void cpu_multithread_renderer::render(scene& _scene, camera& _camera, const renderParameters& _params, std::shared_ptr<sampler> aa_sampler, randomizer& rnd) const
{
	int image_height = _camera.getImageHeight();
	int image_width = _camera.getImageWidth();
	int sqrt_spp = _camera.getSqrtSpp();
	int max_depth = _camera.getMaxDepth();
	int spp = _camera.getSamplePerPixel();

	const unsigned int chunk_per_thread = 4;
	const unsigned int nbr_threads = m_nb_core;


	std::cout << "[INFO] Starting multithreaded rendering" << std::endl;
	std::cout << "[INFO] Using " << nbr_threads << " CPU cores" << std::endl;

	std::vector<std::vector<color>> image(image_height, std::vector<color>(image_width, color()));

	int global_done_scanlines = 0;

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

	#pragma omp parallel num_threads(nbr_threads)
	{
		srand(int(time(NULL)) ^ omp_get_thread_num());

		#pragma omp for schedule(dynamic, image_height/(nbr_threads * chunk_per_thread))
		for (int j = 0; j < image_height; ++j)
		{
			if (!_params.quietMode)
			{
				#pragma omp critical
				{
					if ((image_height - global_done_scanlines) % 10 == 0)
					{
						std::clog << "\rScanlines remaining: " << image_height - global_done_scanlines << " " << std::flush;
					}
				}

				#pragma omp atomic
				++global_done_scanlines;
			}

			for (int i = 0; i < image_width; ++i)
			{
				color pixel_color(0, 0, 0);

				for (int s_j = 0; s_j < sqrt_spp; ++s_j)
				{
					for (int s_i = 0; s_i < sqrt_spp; ++s_i)
					{
						ray r = _camera.get_ray(i, j, s_i, s_j, aa_sampler, rnd);

						// pixel color is progressively being refined
						pixel_color += _camera.ray_color(r, max_depth, _scene, rnd);
					}
				}

				image[j][i] = pixel_color;

				// preview each line when fully calculated
				if (i >= image_width - 1)
				{
					#pragma omp critical
					{
						preview_line(*out, j, image[j], spp, _params.useGammaCorrection);
					}
				}
			}
		}
	}

	// dirty !!! add 2 line of padding to avoid black line in rendered window
	for (int j = 0; j < 2; ++j)
	{
		for (int i = 0; i < image_width; ++i)
		{
			out->write_to_output(i, image_height + j, color::black());
		}
	}

	if (_params.quietMode)
		std::cout << std::endl << "[INFO] Rendering completed !" << std::endl;


	std::cout << std::flush;

	if (!_params.quietMode)
		std::clog << "\r[INFO] Done.                 " << std::endl;


	out->clean_output();


	// save to disk if needed
	if (!_params.saveFilePath.empty())
	{
		if (saveToFile(_params.saveFilePath, image, image_width, image_height, spp))
		{
			std::clog << "[INFO] Image saved to " << _params.saveFilePath << std::endl;
		}
	}
}