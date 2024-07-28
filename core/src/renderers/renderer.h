#pragma once

#include "../misc/scene.h"
#include "../cameras/camera.h"
#include "../misc/renderParameters.h"

class renderer
{
public:
	renderer();
	virtual ~renderer() = default;

	virtual void render(scene& _scene, camera& _camera, const renderParameters& _params, std::shared_ptr<sampler> aa_sampler) const;

protected:
	static void preview_line(int j, std::vector<color> i, int spp);
	static bool saveToFile(string filepath, std::vector<std::vector<color>> image, int width, int height, int spp);
};