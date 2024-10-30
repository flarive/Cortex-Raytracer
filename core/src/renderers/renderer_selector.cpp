#include "renderer_selector.h"

#include "../cameras/camera.h"

#include "../samplers/sampler.h"
#include "../samplers/random_sampler.h"
#include "../samplers/msaa_sampler.h"

#include "../renderers/renderer.h"
#include "../renderers/cpu_singlethread_renderer.h"
#include "../renderers/cpu_multithread_renderer.h"
#include "../renderers/gpu_cuda_renderer.h"


void renderer_selector::render(scene& _scene, const renderParameters& _params, randomizer& rnd)
{
    std::cout << "[INFO] Init scene" << std::endl;



    std::shared_ptr<camera> cam = _scene.get_camera();
    cam->initialize(_params);
	
	_scene.extract_emissive_objects();

    std::cout << "[INFO] Optimizing scene" << std::endl;

	_scene.build_optimized_world(rnd);


    // init default anti aliasing sampler
    std::shared_ptr<sampler> aa = nullptr;

    if (_params.sampler_type == 1)
        aa = std::make_shared<random_sampler>(cam->get_pixel_delta_u(), cam->get_pixel_delta_v(), cam->getSamplePerPixel());
	else if (_params.sampler_type == 2)
		aa = std::make_shared<msaa_sampler>(cam->get_pixel_delta_u(), cam->get_pixel_delta_v(), cam->getSamplePerPixel());


    std::unique_ptr<renderer> r = nullptr;
    

    if (!_params.use_gpu)
    {
        // cpu
        if (_params.nb_cpu_cores > 1)
        {
            r = std::make_unique<cpu_multithread_renderer>(_params.nb_cpu_cores);
        }
        else
        {
            r = std::make_unique<cpu_singlethread_renderer>(1);
        }
    }
    else
    {
		// gpu
		// not implemented yet
        r = std::make_unique<gpu_cuda_renderer>(0);
	}

    if (r)
        r->render(_scene, *cam, _params, aa, rnd);
}