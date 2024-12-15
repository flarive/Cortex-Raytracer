#pragma once

#include <tiny_obj_loader.h>

#include "../primitives/hittable.h"
#include "../materials/material.h"
#include "../utilities/types.h"
#include "../textures/displacement_texture.h"

class obj_mesh_loader
{
public:

    obj_mesh_loader();


    typedef struct
    {
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        tinyobj::attrib_t attributes;
    } obj_mesh_data;


    /// <summary>
    /// https://github.com/Drummersbrother/raytracing-in-one-weekend/blob/90b1d3d7ce7f6f9244bcb925c77baed4e9d51705/rtw_stb_obj_loader.h
    /// </summary>
    /// <param name="filepath"></param>
    /// <param name="model_material"></param>
    /// <param name="use_mtl"></param>
    /// <param name="shade_smooth"></param>
    /// <returns></returns>
    static bool load_model_from_file(std::string filepath, obj_mesh_data& data);

    static std::shared_ptr<hittable> convert_model_from_file(obj_mesh_data& data, std::shared_ptr<material> model_material, bool use_mtl, bool shade_smooth, randomizer& rnd, std::string name = "");


private:

    static std::shared_ptr<material> get_mtl_mat(const tinyobj::material_t& reader_mat);

    static void computeTangentBasis(std::array<vector3, 3>& vertices, std::array<vector2, 3>& uvs, std::array<vector3, 3>& normals, std::array<vector3, 3>& tangents, std::array<vector3, 3>& bitangents);

    static void applyDisplacement(obj_mesh_data& data, std::shared_ptr<displacement_texture> tex);

    static color get_color(tinyobj::real_t* raws);
};   
