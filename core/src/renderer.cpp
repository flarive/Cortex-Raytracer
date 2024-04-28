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

        if (!_params.quietMode)
		    std::clog << "Detected " << n_threads << " concurrent threads." << std::endl;

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
    const int image_height = _camera.getImageHeight();
    const int image_width = _camera.getImageWidth();

    const int sqrt_spp = _camera.getSqrtSpp();
    const int max_depth = _camera.getMaxDepth();

    const int spp = _camera.getSamplePerPixel();
    
    std::vector<std::vector<color>> image(image_height, std::vector<color>(image_width, color()));

    //auto image = new color[288][512];


    // render
    //std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    int global_done_scanlines = 0;

    #pragma omp parallel num_threads(nbr_threads)
    {
        srand(int(time(NULL)) ^ omp_get_thread_num());

        #pragma omp for schedule(dynamic, image_height/(nbr_threads * chunk_per_thread))
        for (int j = 0; j < image_height; ++j)
        {
            //if (!_params.quietMode)
            //{
            //    #pragma omp critical
            //    {
            //        if ((image_height - global_done_scanlines) % 10 == 0)
            //        {
            //            std::clog << "\rScanlines remaining: " << image_height - global_done_scanlines << " " << std::flush;
            //        }
            //    }

            //    #pragma omp atomic
            //    ++global_done_scanlines;
            //}

            for (int i = 0; i < image_width; ++i)
            {
                color pixel_color(0, 0, 0);

                for (int s_j = 0; s_j < sqrt_spp; ++s_j)
                {
                    for (int s_i = 0; s_i < sqrt_spp; ++s_i)
                    {
                        ray r = _camera.get_ray(i, j, s_i, s_j);
                        pixel_color += _camera.ray_color(r, max_depth, _scene);
                    }
                }

                image[j][i] = pixel_color;

                // preview each line when fully calculated
                if (i >= image_width - 1)
                {
                    #pragma omp critical
                    {
                        preview_line(j, image[j], spp);
                    }
                }
            }
        }
    }

    // redraw the whole image at the end
    for (int j = 0; j < image_height ; ++j)
    {
        for (int i = 0; i < image_width; ++i)
        {
            color::write_color(std::cout, i, j, image[j][i], spp);
        }
    }

    std::cout << std::flush;

	if (!_params.quietMode)
		std::clog << "\rDone.                 \n";
}


void renderer::preview_line(int j, std::vector<color> i, int spp)
{
    for (unsigned int n = 0; n < i.size(); n++)
    {
        color::write_color(std::cout, n, j, i[n], spp);
    }
}


//bool saveAsPPM(const FloatImage& image, const std::string& filename)
//{
//    std::ofstream file(filename);
//
//    if (!file.is_open())
//    {
//        return false;
//    }
//
//    file << "P3\n" << image.width() << ' ' << image.height() << "\n255\n";
//
//    for (int i = 0; i < image.height(); i++)
//    {
//        for (int j = 0; j < image.width(); j++)
//        {
//            // Conversion to RGB in [0; 255]
//            const auto red = static_cast<int>(255.0 * std::clamp(image.at(i, j).x, 0.0, 1.0));
//            const auto green = static_cast<int>(255.0 * std::clamp(image.at(i, j).y, 0.0, 1.0));
//            const auto blue = static_cast<int>(255.0 * std::clamp(image.at(i, j).z, 0.0, 1.0));
//
//            file << red << ' ' << green << ' ' << blue << '\n';
//        }
//    }
//
//    file.close();
//
//    return false;
//}
