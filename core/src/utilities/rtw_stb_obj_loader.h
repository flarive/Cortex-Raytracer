#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_DOUBLE
#include "obj/tinyobjloader.hpp"

#include "../bvh_node.h"
#include "../materials/material.h"
#include "../materials/mtl_material.h"
#include "../textures/solid_color_texture.h"
#include "../textures/image_texture.h"
#include "../utilities/types.h"
#include "../primitives/triangle.h"

class rtw_stb_obj_loader
{
public:

    rtw_stb_obj_loader()
    {
    }

    static std::shared_ptr<hittable> load_model_from_file(std::string filename, std::shared_ptr<material> model_material, bool use_mtl, bool shade_smooth)
    {
        // from https://github.com/mojobojo/OBJLoader/blob/master/example.cc
        std::cerr << "Loading .obj file '" << filename << "'." << std::endl;

        std::string inputfile = filename;
        // By default searches for mtl file in same dir as obj file, and triangulates
        tinyobj::ObjReaderConfig reader_config;

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(inputfile, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader error: " << reader.Error();
            }
            exit(1);
        }

        if (!reader.Warning().empty()) {
            std::cerr << "TinyObjReader warning: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& raw_materials = reader.GetMaterials();

        std::vector<std::shared_ptr<material>> converted_mats;
        for (auto& raw_mat : raw_materials) {
            converted_mats.push_back(get_mtl_mat(raw_mat));
        }

        const bool use_mtl_file = use_mtl && (raw_materials.size() != 0);

        hittable_list model_output;

        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {

            hittable_list shape_triangles;
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                const int fv = 3; assert(shapes[s].mesh.num_face_vertices[f] == fv);

                vector3 tri_v[3]; // all vertex
                vector3 tri_vn[3]; // all normals
                vector2 tri_uv[3]; // all uv

                // Loop over vertices in the face.
                for (size_t v = 0; v < 3; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    tri_v[v] = vector3(vx, vy, vz);

                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

                        tri_vn[v] = vector3(nx, ny, nz);
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0) {
                        tinyobj::real_t tu = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        tinyobj::real_t tv = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

                        tri_uv[v] = vector2(tu, tv);
                    }
                }
                std::shared_ptr<material> tri_mat;
                if (use_mtl_file) {
                    tri_mat = converted_mats[shapes[s].mesh.material_ids[f]];
                }
                else {
                    tri_mat = model_material;
                }
                shape_triangles.add(make_shared<triangle>(
                    tri_v[0], tri_v[1], tri_v[2],
                    tri_vn[0], tri_vn[1], tri_vn[2],
                    tri_uv[0], tri_uv[1], tri_uv[2],
                    shade_smooth, tri_mat));

                index_offset += fv;
            }

            //model_output.add(std::make_shared<bvh_node>(shape_triangles, 0, 1));
            model_output.add(std::make_shared<bvh_node>(shape_triangles));
        }

        //return std::make_shared<bvh_node>(model_output, 0, 1);
        return std::make_shared<bvh_node>(model_output);
    }

    static color _getcol(tinyobj::real_t* raws)
    {
        return color(raws[0], raws[1], raws[2]);
    }

    static std::shared_ptr<material> get_mtl_mat(const tinyobj::material_t& reader_mat)
    {
        std::shared_ptr<texture> diffuse_a = nullptr;
        std::shared_ptr<texture> specular_a = nullptr;
        //std::shared_ptr<texture> emissive_a = std::make_shared<solid_color_texture>(_getcol((tinyobj::real_t*)reader_mat.emission));
        std::shared_ptr<texture> emissive_a = std::make_shared<solid_color_texture>(color::black());
        std::shared_ptr<texture> transparency_a = std::make_shared<solid_color_texture>(_getcol((tinyobj::real_t*)reader_mat.transmittance) * (1. - reader_mat.dissolve));
        std::shared_ptr<texture> sharpness_a = std::make_shared<solid_color_texture>(color(1, 0, 0) * reader_mat.shininess);

        // diffuse
        if (reader_mat.diffuse_texname.size() > 0)
        {
            diffuse_a = std::make_shared<image_texture>(reader_mat.diffuse_texname.c_str());
        }
        else
        {
            diffuse_a = std::make_shared<solid_color_texture>(_getcol((tinyobj::real_t*)reader_mat.diffuse));
        }


        // specular
        if (reader_mat.specular_texname.size() > 0)
        {
            specular_a = std::make_shared<image_texture>(reader_mat.specular_texname.c_str());
        }
        else
        {
            specular_a = std::make_shared<solid_color_texture>(_getcol((tinyobj::real_t*)reader_mat.specular));
        }
        

        return make_shared<mtl_material>(
            diffuse_a,
            specular_a,
            emissive_a,
            transparency_a,
            sharpness_a,
            reader_mat.illum);
    }
};

