#pragma once


#include "misc/scene.h"
#include "renderParameters.h"

class renderer
{
public:

	void render(scene& _scene, const renderParameters& _params, bool _multithreaded);

	void render_single_thread(scene& _scene, const renderParameters& _params);

};

