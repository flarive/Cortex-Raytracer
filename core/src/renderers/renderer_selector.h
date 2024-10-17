#pragma once

#include "../misc/scene.h"
#include "../misc/renderParameters.h"

class renderer_selector
{
public:

	void render(scene& _scene, const renderParameters& _params);
};