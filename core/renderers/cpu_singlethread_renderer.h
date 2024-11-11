#pragma once

#include "renderer.h"

class cpu_singlethread_renderer : public renderer
{
public:
	cpu_singlethread_renderer(unsigned int nb_cores);
	void render(scene& _scene, camera& _camera, const renderParameters& _params, std::shared_ptr<sampler> aa_sampler, randomizer& rnd) const override;
};