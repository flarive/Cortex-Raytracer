#pragma once

#include "misc/scene.h"
#include "cameras/camera.h"
#include "utilities/random.h"
#include "renderParameters.h"

class renderer
{
public:

	void render(scene& _scene, camera& cam, const renderParameters& _params, bool _multithreaded = true);

	void render_single_thread(scene& _scene, camera& _camera, const renderParameters& _params, Random& random);
	void render_multi_thread(scene& _scene, camera& _camera, const renderParameters& _params, const int nbr_threads, const int chunk_per_thread, Random& random);



private:
	void preview_line(int j, std::vector<color> i, int spp);
	bool saveToFile(string filepath, std::vector<std::vector<color>> image, int width, int height, int spp);
};