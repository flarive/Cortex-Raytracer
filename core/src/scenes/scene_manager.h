#pragma once

#include "../cameras/perspective_camera.h"
#include "../cameras/orthographic_camera.h"

class scene_manager
{
public:
    scene load_scene(const renderParameters& params, randomizer& rnd);
};