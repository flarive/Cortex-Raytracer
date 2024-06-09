#include "rtw_stb_obj_loader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_DOUBLE
#include "obj/tinyobjloader.hpp"

#include "../textures/solid_color_texture.h"
#include "../textures/image_texture.h"
#include "../textures/bump_texture.h"
#include "../textures/normal_texture.h"
#include "../materials//phong.h"

#include <array>
#include <filesystem>


rtw_stb_obj_loader::rtw_stb_obj_loader()
{
}


/// <summary>
/// https://github.com/Drummersbrother/raytracing-in-one-weekend/blob/90b1d3d7ce7f6f9244bcb925c77baed4e9d51705/rtw_stb_obj_loader.h
/// </summary>
/// <param name="filename"></param>
/// <param name="model_material"></param>
/// <param name="use_mtl"></param>
/// <param name="shade_smooth"></param>
/// <returns></returns>
std::shared_ptr<hittable> rtw_stb_obj_loader::load_model_from_file(std::string filepath, std::shared_ptr<material> model_material, bool use_mtl, bool shade_smooth, std::string name)
{
    // from https://github.com/mojobojo/OBJLoader/blob/master/example.cc
    std::filesystem::path dir(std::filesystem::current_path());
    std::filesystem::path file(filepath);
    std::filesystem::path fullexternalProgramPath = dir / file;

    std::cout << "[INFO] Loading obj file " << fullexternalProgramPath.filename() << std::endl;

    auto fullAbsPath = std::filesystem::absolute(fullexternalProgramPath);

    if (!std::filesystem::exists(fullAbsPath))
    {
        std::cout << "[ERROR] obj file not found ! " << fullAbsPath.generic_string() << std::endl;
        return nullptr;
    }
    

    std::string inputfile = fullAbsPath.generic_string();
    // By default searches for mtl file in same dir as obj file, and triangulates
    tinyobj::ObjReaderConfig reader_config;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "[ERROR] Loading obj file error: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty())
    {
        std::cerr << "[WARN] Loading obj file warning: " << reader.Warning();
    }

    hittable_list model_output;

    try
    {

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& raw_materials = reader.GetMaterials();

        std::vector<std::shared_ptr<material>> converted_mats;
        for (auto& raw_mat : raw_materials)
        {
            converted_mats.push_back(get_mtl_mat(raw_mat));
        }

        const bool use_mtl_file = use_mtl && (raw_materials.size() != 0);

        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++)
        {
            hittable_list shape_triangles;
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                const int fv = 3; assert(shapes[s].mesh.num_face_vertices[f] == fv);

                std::array<vector3, 3> tri_v;
                std::array<vector3, 3> tri_vn;
                std::array<vector2, 3> tri_uv;

                // Loop over vertices in the face.
                for (size_t v = 0; v < 3; v++)
                {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    tri_v[v] = vector3(vx, vy, vz);

                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0)
                    {
                        tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];

                        tri_vn[v] = vector3(nx, ny, nz);
                    }

                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                    if (idx.texcoord_index >= 0)
                    {
                        tinyobj::real_t tu = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        tinyobj::real_t tv = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];

                        tri_uv[v] = vector2(tu, tv);
                    }
                }

                // Calculate tangent and bitangent for normal texture
                std::array<vector3, 3> tri_tan; // triangle tangents
                std::array<vector3, 3> tri_bitan; // triangle bitangents
                computeTangentBasis(tri_v, tri_uv, tri_vn, tri_tan, tri_bitan);

                std::shared_ptr<material> tri_mat;
                if (use_mtl_file)
                {
                    tri_mat = converted_mats[shapes[s].mesh.material_ids[f]];
                }
                else
                {
                    tri_mat = model_material;
                }

                shape_triangles.add(make_shared<triangle>(
                    tri_v[0], tri_v[1], tri_v[2],
                    tri_vn[0], tri_vn[1], tri_vn[2],
                    tri_uv[0], tri_uv[1], tri_uv[2],
                    tri_tan[0], tri_tan[1], tri_tan[2],
                    tri_bitan[0], tri_bitan[1], tri_bitan[2],
                    shade_smooth, tri_mat));

                index_offset += fv;
            }

            // group all object triangles in a bvh node
            //model_output.add(std::make_shared<bvh_node>(shape_triangles, 0, 1));
            model_output.add(std::make_shared<bvh_node>(shape_triangles, name));
        }
    }
    catch (const std::exception&)
    {
        std::cerr << "[ERROR] Parsing obj file failed: " << reader.Error() << std::endl;
        exit(1);
    }

    // group all objects in the .obj file in a single bvh node
    //return std::make_shared<bvh_node>(model_output, 0, 1);
    return std::make_shared<bvh_node>(model_output, name);
}

/// <summary>
/// https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/#computing-the-tangents-and-bitangents
/// </summary>
/// <param name="vertices"></param>
/// <param name="uvs"></param>
/// <param name="normals"></param>
/// <param name="tangents"></param>
/// <param name="bitangents"></param>
void rtw_stb_obj_loader::computeTangentBasis(std::array<vector3 ,3>& vertices, std::array<vector2, 3>& uvs, std::array<vector3, 3>& normals, std::array<vector3, 3>& tangents, std::array<vector3, 3>& bitangents)
{
    //For each triangle, we compute the edge(deltaPos) and the deltaUV
    for (int i = 0; i < vertices.size(); i += 3)
    {
        // Shortcuts for vertices
        vector3& v0 = vertices[i + 0];
        vector3& v1 = vertices[i + 1];
        vector3& v2 = vertices[i + 2];

        // Shortcuts for UVs
        vector2& uv0 = uvs[i + 0];
        vector2& uv1 = uvs[i + 1];
        vector2& uv2 = uvs[i + 2];

        // Edges of the triangle : position delta
        vector3 deltaPos1 = v1 - v0;
        vector3 deltaPos2 = v2 - v0;

        // UV delta
        vector2 deltaUV1 = uv1 - uv0;
        vector2 deltaUV2 = uv2 - uv0;

		double r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		vector3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        vector3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		// Set the same tangent for all three vertices of the triangle.
		// They will be merged later, in vboindexer.cpp
        tangents[0] = tangent;
        tangents[1] = tangent;
        tangents[2] = tangent;

		// Same thing for bitangents
		bitangents[0] = bitangent;
        bitangents[1] = bitangent;
        bitangents[2] = bitangent;
    }
}

color rtw_stb_obj_loader::get_color(tinyobj::real_t* raws)
{
    return color(raws[0], raws[1], raws[2]);
}


std::shared_ptr<material> rtw_stb_obj_loader::get_mtl_mat(const tinyobj::material_t& reader_mat)
{
    std::shared_ptr<texture> diffuse_a = nullptr;
    std::shared_ptr<texture> specular_a = nullptr;
    std::shared_ptr<texture> bump_a = nullptr;
    std::shared_ptr<texture> normal_a = nullptr;
    //std::shared_ptr<texture> emissive_a = std::make_shared<solid_color_texture>(get_color((tinyobj::real_t*)reader_mat.emission));
    std::shared_ptr<texture> emissive_a = std::make_shared<solid_color_texture>(color::black());
    std::shared_ptr<texture> transparency_a = std::make_shared<solid_color_texture>(get_color((tinyobj::real_t*)reader_mat.transmittance) * (1. - reader_mat.dissolve));
    std::shared_ptr<texture> sharpness_a = std::make_shared<solid_color_texture>(color(1, 0, 0) * reader_mat.shininess);

    double shininess = reader_mat.shininess;

    // diffuse
    if (reader_mat.diffuse_texname.size() > 0)
    {
        diffuse_a = std::make_shared<image_texture>(reader_mat.diffuse_texname);
    }
    else
    {
        diffuse_a = std::make_shared<solid_color_texture>(get_color((tinyobj::real_t*)reader_mat.diffuse));
    }


    // specular
    if (reader_mat.specular_texname.size() > 0)
    {
        specular_a = std::make_shared<image_texture>(reader_mat.specular_texname);
    }
    else
    {
        specular_a = std::make_shared<solid_color_texture>(get_color((tinyobj::real_t*)reader_mat.specular));
    }

    // bump
    if (reader_mat.bump_texname.size() > 0)
    {
        auto bump_tex = std::make_shared<image_texture>(reader_mat.bump_texname);
        bump_a = std::make_shared<bump_texture>(bump_tex, 10);
    }

    // normal
    if (reader_mat.normal_texname.size() > 0)
    {
        auto normal_tex = std::make_shared<image_texture>(reader_mat.normal_texname);
        normal_a = std::make_shared<normal_texture>(normal_tex);
    }


    //return make_shared<mtl_material>(
    //    diffuse_a,
    //    specular_a,
    //    bump_a,
    //    normal_a,
    //    emissive_a,
    //    transparency_a,
    //    sharpness_a,
    //    reader_mat.illum);

    return std::make_shared<phong>(diffuse_a, specular_a, bump_a, normal_a, color(1.0, 1.0, 1.0), shininess);
}