#pragma once

#include "../misc/scene.h"
#include "../cameras/camera.h"
#include "../misc/renderParameters.h"
#include "../outputs/output.h"

class renderer
{
public:
	renderer(unsigned int nb_cores);
	virtual ~renderer() = default;

	virtual void render(scene& _scene, camera& _camera, const renderParameters& _params, std::shared_ptr<sampler> aa_sampler) const;

private:
	

protected:
	unsigned int m_nb_core = 1;
	
	static void preview_line(const output& out, int j, std::vector<color> i, int spp);
	static bool saveToFile(string filepath, std::vector<std::vector<color>> image, int width, int height, int spp);
};