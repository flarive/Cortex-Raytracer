#pragma once

#include "../primitives/triangle.h"
#include "../textures/image_texture.h"
#include "../textures/displacement_texture.h"
#include "../materials/phong_material.h"
#include "../misc/bvh_node.h"
#include "../misc/singleton.h"

#include "openfbx/includes/ofbx.h"

#include <memory>
#include <string>


class fbx_mesh_loader
{
public:
    fbx_mesh_loader();

    typedef struct
    {
        const ofbx::IScene* scene;
    } fbx_mesh_data;

    static bool load_model_from_file(const std::string& filepath, fbx_mesh_data& data);

    static std::shared_ptr<hittable> convert_model_from_file(fbx_mesh_data& data, randomizer& rnd, std::string name = "");

    


private:

    static void computeTangentBasis(std::array<vector3, 3>& vertices, std::array<vector2, 3>& uvs, std::array<vector3, 3>& normals, std::array<vector3, 3>& tangents, std::array<vector3, 3>& bitangents);

};