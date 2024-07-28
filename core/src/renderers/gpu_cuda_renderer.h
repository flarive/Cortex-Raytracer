#pragma once

#include "renderer.h"

class gpu_cuda_renderer : public renderer
{
public:
	void render(scene& _scene, camera& _camera, const renderParameters& _params, std::shared_ptr<sampler> aa_sampler) const override;
};