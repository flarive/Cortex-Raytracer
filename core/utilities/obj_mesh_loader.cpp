#include "obj_mesh_loader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_DOUBLE
#include <tiny_obj_loader.h>

#include "../primitives/triangle.h"
#include "../textures/solid_color_texture.h"
#include "../textures/image_texture.h"
#include "../textures/bump_texture.h"
#include "../textures/normal_texture.h"
#include "../textures/displacement_texture.h"
#include "../materials/phong_material.h"
#include "../misc/bvh_node.h"

#include <array>
#include <filesystem>


obj_mesh_loader::obj_mesh_loader()
{
}


/// <summary>
/// https://github.com/Drummersbrother/raytracing-in-one-weekend/blob/90b1d3d7ce7f6f9244bcb925c77baed4e9d51705/rtw_stb_obj_loader.h
/// </summary>
/// <param name="filename"></param>
/// <param name="model_material"></param>
/// <param name="use_mtl"></param>
/// <param name="shade_smooth"></param>
bool obj_mesh_loader::load_model_from_file(std::string filepath, obj_mesh_data& data)
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
        return false;
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

    try
    {
        data.attributes = reader.GetAttrib();
        data.shapes = reader.GetShapes();
        data.materials = reader.GetMaterials();
        
        return true;
    }
    catch (const std::exception&)
    {
        std::cerr << "[ERROR] Loading obj file failed: " << reader.Error() << std::endl;
        exit(1);
    }

    return false;
}


std::shared_ptr<hittable> obj_mesh_loader::convert_model_from_file(obj_mesh_data& data, std::shared_ptr<material> model_material, bool use_mtl, bool shade_smooth, randomizer& rnd, std::string name)
{
    hittable_list model_output;

    std::cout << "[INFO] Start building obj file (" << data.shapes.size() << " objects found)" << std::endl;

    std::vector<std::shared_ptr<material>> converted_mats;

    const bool use_mtl_file = use_mtl && data.materials.size() > 0;

    if (use_mtl_file)
    {
        for (auto& raw_mat : data.materials)
        {
            converted_mats.push_back(get_mtl_mat(raw_mat));
        }
    }

    // Loop over shapes (meshes)
    for (size_t s = 0; s < data.shapes.size(); s++)
    {
        if (use_mtl_file)
        {
            std::string filepath = data.materials[0].displacement_texname;
            double strength = data.materials[0].displacement_texopt.bump_multiplier;

            if (!filepath.empty())
            {
                auto image_tex = std::make_shared<image_texture>(filepath);
                auto displace_texture = std::make_shared<displacement_texture>(image_tex, strength);
                if (displace_texture)
                {
                    obj_mesh_loader::applyDisplacement(data, displace_texture);
                }
            }
        }
        else if (model_material && model_material->has_displace_texture())
        {
            auto displace_texture = std::dynamic_pointer_cast<displacement_texture>(model_material->get_displacement_texture());
            if (displace_texture)
            {
                obj_mesh_loader::applyDisplacement(data, displace_texture);
            }
        }
        
        size_t index_offset = 0;

        // Loop over faces (triangles)
        for (size_t f = 0; f < data.shapes[s].mesh.num_face_vertices.size(); f++)
        {
            hittable_list shape_triangles;
            
            const int fv = 3;
            
            // Only accept triangles
            assert(data.shapes[s].mesh.num_face_vertices[f] == fv);

            std::array<vector3, 3> tri_v{};
            std::array<vector3, 3> tri_vn{};
            std::array<vector2, 3> tri_uv{};

            // Loop over vertices in the face.
            for (size_t v = 0; v < 3; v++)
            {
                tinyobj::index_t idx = data.shapes[s].mesh.indices[index_offset + v];

                // access to vertices
                tinyobj::real_t vx = data.attributes.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = data.attributes.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = data.attributes.vertices[3 * size_t(idx.vertex_index) + 2];

                tri_v[v] = vector3(vx, vy, vz);

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0)
                {
                    tinyobj::real_t nx = data.attributes.normals[3 * size_t(idx.normal_index) + 0];
                    tinyobj::real_t ny = data.attributes.normals[3 * size_t(idx.normal_index) + 1];
                    tinyobj::real_t nz = data.attributes.normals[3 * size_t(idx.normal_index) + 2];

                    tri_vn[v] = vector3(nx, ny, nz);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0)
                {
                    tinyobj::real_t tu = data.attributes.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t tv = data.attributes.texcoords[2 * size_t(idx.texcoord_index) + 1];

                    tri_uv[v] = vector2(tu, tv);
                }
            }

            // Calculate tangent and bitangent for normal texture
            std::array<vector3, 3> tri_tan{}; // triangle tangents
            std::array<vector3, 3> tri_bitan{}; // triangle bitangents
            computeTangentBasis(tri_v, tri_uv, tri_vn, tri_tan, tri_bitan);

            std::shared_ptr<material> tri_mat;
            if (use_mtl_file)
            {
                tri_mat = converted_mats[data.shapes[s].mesh.material_ids[f]];
            }
            else
            {
                tri_mat = model_material;
            }

            shape_triangles.add(make_shared<triangle>(
                s,
                f,
                tri_v[0], tri_v[1], tri_v[2],
                tri_vn[0], tri_vn[1], tri_vn[2],
                tri_uv[0], tri_uv[1], tri_uv[2],
                tri_tan[0], tri_tan[1], tri_tan[2],
                tri_bitan[0], tri_bitan[1], tri_bitan[2],
                shade_smooth, tri_mat));

            index_offset += fv;

            // group all object triangles in a bvh node
            model_output.add(std::make_shared<bvh_node>(shape_triangles, rnd, name));
        }

        std::cout << "[INFO] Parsing obj file (object name " << data.shapes[s].name << " / " << static_cast<int>(data.attributes.vertices.size() / 3) << " vertex / " << data.shapes[s].mesh.num_face_vertices.size() << " faces)" << std::endl;
    }

    std::cout << "[INFO] End building obj file" << std::endl;


    // group all objects in the .obj file in a single bvh node
    return std::make_shared<bvh_node>(model_output, rnd, name);
}

/// <summary>
/// https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/#computing-the-tangents-and-bitangents
/// </summary>
/// <param name="vertices"></param>
/// <param name="uvs"></param>
/// <param name="normals"></param>
/// <param name="tangents"></param>
/// <param name="bitangents"></param>
void obj_mesh_loader::computeTangentBasis(std::array<vector3 ,3>& vertices, std::array<vector2, 3>& uvs, std::array<vector3, 3>& normals, std::array<vector3, 3>& tangents, std::array<vector3, 3>& bitangents)
{
    //For each triangle, we compute the edge(deltaPos) and the deltaUV
    for (uint64_t i = 0; i < vertices.size(); i += 3)
    {
        // Shortcuts for vertices
        vector3& v0 = vertices[i];
        vector3& v1 = vertices[i + 1];
        vector3& v2 = vertices[i + 2];

        // Shortcuts for UVs
        vector2& uv0 = uvs[i];
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

color obj_mesh_loader::get_color(tinyobj::real_t* raws)
{
    return color(raws[0], raws[1], raws[2]);
}


std::shared_ptr<material> obj_mesh_loader::get_mtl_mat(const tinyobj::material_t& reader_mat)
{
    color ambient(0.0, 0.0, 0.0);
    std::shared_ptr<texture> diffuse_a = nullptr;
    std::shared_ptr<texture> specular_a = nullptr;
    std::shared_ptr<texture> bump_a = nullptr;
    std::shared_ptr<texture> normal_a = nullptr;
    std::shared_ptr<texture> displace_a = nullptr;
    std::shared_ptr<texture> alpha_a = nullptr;
    //std::shared_ptr<texture> emissive_a = std::make_shared<solid_color_texture>(get_color((tinyobj::real_t*)reader_mat.emission));
    std::shared_ptr<texture> emissive_a = nullptr;
    std::shared_ptr<texture> transparency_a = std::make_shared<solid_color_texture>(get_color((tinyobj::real_t*)reader_mat.transmittance) * (1. - reader_mat.dissolve));
    std::shared_ptr<texture> sharpness_a = std::make_shared<solid_color_texture>(color(1, 0, 0) * reader_mat.shininess);

    // Ns
    double shininess = reader_mat.shininess; // 0.0

    // diffuse
    // map_Kd ..\..\data\models\crate_diffuse.jpg
    if (!reader_mat.diffuse_texname.empty())
    {
        diffuse_a = std::make_shared<image_texture>(reader_mat.diffuse_texname);
    }
    else
    {
        diffuse_a = std::make_shared<solid_color_texture>(get_color((tinyobj::real_t*)reader_mat.diffuse));
    }


    // specular
    // map_Ks ..\..\data\models\crate_roughness.jpg
    if (!reader_mat.specular_texname.empty())
    {
        specular_a = std::make_shared<image_texture>(reader_mat.specular_texname);
    }
    else
    {
        specular_a = std::make_shared<solid_color_texture>(get_color((tinyobj::real_t*)reader_mat.specular));
    }

    // bump
    // map_bump -bm 0.3000 ..\..\data\models\crate_bump.jpg
    if (!reader_mat.bump_texname.empty())
    {
        // bump strength
        auto bump_m = reader_mat.bump_texopt.bump_multiplier;

        // bump texture
        auto bump_tex = std::make_shared<image_texture>(reader_mat.bump_texname);
        bump_a = std::make_shared<bump_texture>(bump_tex, bump_m);
    }

    // normal
    // norm -bm 0.3000 ..\..\data\models\crate_normal.jpg
    if (!reader_mat.normal_texname.empty())
    {
        // normal strength
        double normal_m = reader_mat.normal_texopt.bump_multiplier;
        
        // normal texture
        auto normal_tex = std::make_shared<image_texture>(reader_mat.normal_texname);
        normal_a = std::make_shared<normal_texture>(normal_tex, normal_m);
    }

    // emissive
    // 
    if (!reader_mat.emissive_texname.empty())
    {
        
    }

    // displacement/height
    // disp -bm 1.0 ..\..\data\models\rocky_normal.jpg
    if (!reader_mat.displacement_texname.empty())
    {
        // displace strength
        double displace_m = (double)reader_mat.displacement_texopt.bump_multiplier;
        
        // displace texture
        auto displace_tex = std::make_shared<image_texture>(reader_mat.displacement_texname);
        displace_a = std::make_shared<displacement_texture>(displace_tex, displace_m);
    }

    return std::make_shared<phong_material>(diffuse_a, specular_a, bump_a, normal_a, displace_a, alpha_a, emissive_a, ambient, shininess);
}

void obj_mesh_loader::applyDisplacement(obj_mesh_data& data, std::shared_ptr<displacement_texture> tex)
{
    std::cout << "[INFO] Start applying model displacement " << data.shapes.size() << std::endl;

    // temp dic to take each vertex only one time
    std::map<int, bool> dic;

    for (auto& shape : data.shapes)
    {
        for (size_t i = 0; i < shape.mesh.indices.size(); i++)
        {
            auto& idx = shape.mesh.indices[i];

            if (dic.find(idx.vertex_index) == dic.end())
            {
                // dic does not contain vertex yet
                dic.emplace(idx.vertex_index, true);

                float vx = data.attributes.vertices[3 * idx.vertex_index + 0];
                float vy = data.attributes.vertices[3 * idx.vertex_index + 1];
                float vz = data.attributes.vertices[3 * idx.vertex_index + 2];

                double tx = data.attributes.texcoords[2 * idx.texcoord_index + 0];
                double ty = data.attributes.texcoords[2 * idx.texcoord_index + 1];


                color displacement = tex->value(tx, ty, point3());


                vx += vx * (float)displacement.r();
                vy += vy * (float)displacement.g();
                vz += vz * (float)displacement.b();

                data.attributes.vertices[3 * idx.vertex_index + 0] = vx;
                data.attributes.vertices[3 * idx.vertex_index + 1] = vy;
                data.attributes.vertices[3 * idx.vertex_index + 2] = vz;
            }
        }
    }

    //dic.empty();

    std::cout << "[INFO] End applying model displacement" << std::endl;
}
