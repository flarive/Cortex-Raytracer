#include "srenderer.h"

#include "cameras/camera.h"

#include "samplers/sampler.h"
#include "samplers/random_sampler.h"
#include "samplers/msaa_sampler.h"

#include "renderers/renderer.h"
#include "renderers/cpu_singlethread_renderer.h"
#include "renderers/cpu_multithread_renderer.h"
#include "renderers/gpu_cuda_renderer.h"


void srenderer::render(scene& _scene, const renderParameters& _params)
{
    std::cout << "[INFO] Init scene" << std::endl;



    std::shared_ptr<camera> cam = _scene.get_camera();
    cam->initialize(_params);
	
	_scene.extract_emissive_objects();

    std::cout << "[INFO] Optimizing scene" << std::endl;

	_scene.build_optimized_world();


    // init default anti aliasing sampler
    auto sampler = std::make_shared<random_sampler>(cam->get_pixel_delta_u(), cam->get_pixel_delta_v(), cam->getSamplePerPixel());



    std::unique_ptr<renderer> r;
    

    if (!_params.use_gpu)
    {
        // cpu
        if (_params.use_multi_thread)
        {
            r = std::make_unique<cpu_multithread_renderer>();
        }
        else
        {
            r = std::make_unique<cpu_singlethread_renderer>();
        }
    }
    else
    {
		// gpu
        r = std::make_unique<gpu_cuda_renderer>();
	}

    if (r)
        r->render(_scene, *cam, _params, sampler);
}