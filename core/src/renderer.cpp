#include "renderer.h"

#include "misc/color.h"
#include "misc/ray.h"
#include "cameras/camera.h"
#include "utilities/bitmap_image.h"

//#include <omp.h>
#include <thread>


void renderer::render(scene& _scene, camera& _camera, const renderParameters& _params, bool _multithreaded)
{
    std::cout << "[INFO] Init scene" << std::endl;
    
    _camera.initialize(_params);
	
	_scene.extract_emissive_objects();

    std::cout << "[INFO] Optimizing scene" << std::endl;

	_scene.build_optimized_world();

    randomizer initialSeed;

	if (_multithreaded)
	{
		const unsigned int CHUNKS_PER_THREAD = 4;

        const unsigned int n_threads = std::thread::hardware_concurrency();

        //std::vector<Random> randomEngines;
        //for (int i = 0; i < n_threads; ++i)
        //    randomEngines.emplace_back(static_cast<int>(initialSeed.getZeroOne() * 1E5));

        if (!_params.quietMode)
		    std::clog << "Detected " << n_threads << " concurrent threads." << std::endl;

		render_multi_thread(_scene, _camera, _params, n_threads, CHUNKS_PER_THREAD, initialSeed);
	}
	else
	{
		render_single_thread(_scene, _camera, _params, initialSeed);
	}
}


void renderer::render_single_thread(scene& _scene, camera& _camera, const renderParameters& _params, randomizer& random)
{
    const int image_height = _camera.getImageHeight();
    const int image_width = _camera.getImageWidth();
    const int sqrt_spp = _camera.getSqrtSpp();
    const int max_depth = _camera.getMaxDepth();
    const int spp = _camera.getSamplePerPixel();

    std::cout << "[INFO] Starting single thread rendering" << std::endl;

    std::vector<std::vector<color>> image(image_height, std::vector<color>(image_width, color()));

	for (int j = 0; j < _camera.getImageHeight(); ++j)
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
					ray r = _camera.get_ray(i, j, s_i, s_j);

					// pixel color is progressively being refined
					pixel_color += _camera.ray_color(r, max_depth, _scene, random);

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


/// <summary>
/// https://github.com/Drummersbrother/raytracing-in-one-weekend
/// </summary>
/// <param name="_scene"></param>
/// <param name="_params"></param>
void renderer::render_multi_thread(scene& _scene, camera& _camera, const renderParameters& _params, const int nbr_threads, const int chunk_per_thread, randomizer& random)
{
    const int image_height = _camera.getImageHeight();
    const int image_width = _camera.getImageWidth();
    const int sqrt_spp = _camera.getSqrtSpp();
    const int max_depth = _camera.getMaxDepth();
    const int spp = _camera.getSamplePerPixel();

    std::cout << "[INFO] Starting multithreaded rendering" << std::endl;
    std::cout << "[INFO] Using " << nbr_threads << " CPU cores" << std::endl;
    
    std::vector<std::vector<color>> image(image_height, std::vector<color>(image_width, color()));

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

                for (int s_j = 0; s_j < sqrt_spp; ++s_j)
                {
                    for (int s_i = 0; s_i < sqrt_spp; ++s_i)
                    {
                        ray r = _camera.get_ray(i, j, s_i, s_j);
                        pixel_color += _camera.ray_color(r, max_depth, _scene, random);
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

    // dirty !!! add 1 line of padding to avoid black line in rendered window
    for (int j = 0; j < 1 ; ++j)
    {
        for (int i = 0; i < image_width; ++i)
        {
            color::write_color(std::cout, i, image_height + j, color::black(), spp);
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