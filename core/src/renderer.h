#pragma once

#include "misc/scene.h"
#include "cameras/camera.h"
#include "utilities/randomizer.h"
#include "utilities/randomizer2.h"
#include "misc/renderParameters.h"

class renderer
{
public:

	void render(scene& _scene, const renderParameters& _params, bool _multithreaded = true);

	void render_single_thread(scene& _scene, camera& _camera, const renderParameters& _params, randomizer2& random, std::shared_ptr<sampler> aa_sampler);
	void render_multi_thread(scene& _scene, camera& _camera, const renderParameters& _params, const int nbr_threads, const int chunk_per_thread, randomizer2& random, std::shared_ptr<sampler> aa_sampler);



private:
	void preview_line(int j, std::vector<color> i, int spp);
	bool saveToFile(string filepath, std::vector<std::vector<color>> image, int width, int height, int spp);
};