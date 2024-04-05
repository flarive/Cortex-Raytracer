#include "renderer.h"

#include "misc/color.h"
#include "misc/ray.h"
#include "cameras/camera.h"

#include <omp.h>
#include <thread>


void renderer::render(scene& _scene, camera& _camera, const renderParameters& _params, bool _multithreaded)
{
	_camera.initialize(_params);
	
	_scene.extract_lights();

	_scene.build_optimized_world();

	if (_multithreaded)
	{
		const unsigned int CHUNKS_PER_THREAD = 4;

		const unsigned int n_threads = std::thread::hardware_concurrency();
		std::cout << "Detected " << n_threads << " concurrent threads." << std::endl;

		render_multi_thread(_scene, _camera, _params, n_threads, CHUNKS_PER_THREAD);
	}
	else
	{
		render_single_thread(_scene, _camera, _params);
	}
}


void renderer::render_single_thread(scene& _scene, camera& _camera, const renderParameters& _params)
{
	// write ppm file header
	//std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = 0; j < _camera.getImageHeight(); ++j)
	{
		if (!_params.quietMode)
			std::clog << "\rScanlines remaining: " << (_camera.getImageHeight() - j) << ' ' << std::flush;

		for (int i = 0; i < _camera.getImageWidth(); ++i)
		{
			// each pixel is black at the beginning
			color pixel_color(0, 0, 0);

			for (int s_j = 0; s_j < _camera.getSqrtSpp(); ++s_j)
			{
				for (int s_i = 0; s_i < _camera.getSqrtSpp(); ++s_i)
				{
					ray r = _camera.get_ray(i, j, s_i, s_j);

					// pixel color is progressively being refined
					pixel_color += _camera.ray_color(r, _camera.getMaxDepth(), _scene);
				}
			}

			// write ppm file color entry
			color::write_color(std::cout, i, j, pixel_color, _camera.getSamplePerPixel());
		}
	}

	if (!_params.quietMode)
		std::clog << "\rDone.                 \n";
}


/// <summary>
/// https://github.com/Drummersbrother/raytracing-in-one-weekend
/// </summary>
/// <param name="_scene"></param>
/// <param name="_params"></param>
void renderer::render_multi_thread(scene& _scene, camera& _camera, const renderParameters& _params, const int nbr_threads, const int chunk_per_thread)
{
    int image_height = _camera.getImageHeight();
    int image_width = _camera.getImageWidth();
    
    std::vector<std::vector<color>> image(image_height, std::vector<color>(image_width, color()));


    // render
    //std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    int global_done_scanlines = 0;

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

                for (int s_j = 0; s_j < _camera.getSqrtSpp(); ++s_j)
                {
                    for (int s_i = 0; s_i < _camera.getSqrtSpp(); ++s_i)
                    {
                        ray r = _camera.get_ray(i, j, s_i, s_j);
                        pixel_color += _camera.ray_color(r, _camera.getMaxDepth(), _scene);
                        //color ray_contribution = ray_color(r, max_depth, _scene);
                        //zero_nan_vals(ray_contribution);
                        //pixel_color += ray_contribution;
                    }
                }

                image[j][i] = pixel_color;

                if (i >= image_width - 1)
                {
                    #pragma omp critical
                    {
                        render_line(j, image[j], _camera.getSamplePerPixel());
                    }
                }
            }
        }
    }

	if (!_params.quietMode)
		std::clog << "\rDone.                 \n";
}


void renderer::render_line(int j, std::vector<color> i, int spp)
{
    for (unsigned int n = 0; n < i.size(); n++)
    {
        color::write_color(std::cout, n, j, i[n], spp);
    }
}

void renderer::zero_nan_vals(color& v)
{
    if (std::isnan(v.r())) v.r(1.0);
    if (std::isnan(v.g())) v.r(1.0);
    if (std::isnan(v.b())) v.r(1.0);
}
