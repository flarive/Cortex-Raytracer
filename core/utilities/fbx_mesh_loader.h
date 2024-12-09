#pragma once

#include "../primitives/triangle.h"
#include "../textures/image_texture.h"
#include "../textures/displacement_texture.h"
#include "../materials/phong_material.h"
#include "../misc/bvh_node.h"
#include "../misc/singleton.h"

#include <ofbx.h>

#include <memory>
#include <string>


class fbx_mesh_loader
{
public:
    fbx_mesh_loader();

    bool load_model_from_file(const std::string& filepath);


private:

};