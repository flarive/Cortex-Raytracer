#pragma once

//#define TINYOBJLOADER_IMPLEMENTATION
//#define TINYOBJLOADER_USE_DOUBLE
#include "obj/tinyobjloader.hpp"

#include "../bvh_node.h"
#include "../materials/material.h"
#include "../materials/mtl_material.h"
#include "../textures/solid_color_texture.h"
#include "../textures/image_texture.h"
#include "../textures/bump_texture.h"
#include "../utilities/types.h"
#include "../primitives/triangle.h"

class rtw_stb_obj_loader
{
public:

    rtw_stb_obj_loader();

    /// <summary>
    /// https://github.com/Drummersbrother/raytracing-in-one-weekend/blob/90b1d3d7ce7f6f9244bcb925c77baed4e9d51705/rtw_stb_obj_loader.h
    /// </summary>
    /// <param name="filename"></param>
    /// <param name="model_material"></param>
    /// <param name="use_mtl"></param>
    /// <param name="shade_smooth"></param>
    /// <returns></returns>
    static std::shared_ptr<hittable> load_model_from_file(std::string filename, std::shared_ptr<material> model_material, bool use_mtl, bool shade_smooth);

    static color _getcol(tinyobj::real_t* raws);

    static std::shared_ptr<material> get_mtl_mat(const tinyobj::material_t& reader_mat);
};   
