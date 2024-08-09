#pragma once

#include "misc/scene.h"
//#include "cameras/camera.h"
#include "misc/renderParameters.h"

class srenderer
{
public:

	void render(scene& _scene, const renderParameters& _params);
};