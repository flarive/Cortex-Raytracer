#include "gpu_cuda_renderer.h"


// Declaration of the CUDA kernel launcher
//

gpu_cuda_renderer::gpu_cuda_renderer(unsigned int nb_cores) : renderer(nb_cores)
{
}

void gpu_cuda_renderer::render(scene& _scene, camera& _camera, const renderParameters& _params, std::shared_ptr<sampler> aa_sampler, randomizer& rnd) const
{
	if (!_params.quietMode)
		std::clog << "Using GPU Cuda renderer" << std::endl;

	std::cout << "[INFO] Starting GPU Cuda rendering" << std::endl;

	//launchGPU(_scene, _camera, _params, aa_sampler);
}