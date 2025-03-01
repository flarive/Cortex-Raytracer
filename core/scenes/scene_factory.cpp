#include "scene_factory.h"


#include "../utilities/obj_mesh_loader.h"
#include "../utilities/fbx_mesh_loader.h"


#include "../primitives/box.h"
#include "../primitives/cone.h"
#include "../primitives/sphere.h"
#include "../primitives/cylinder.h"
#include "../primitives/disk.h"
#include "../primitives/torus.h"
#include "../primitives/aarect.h"
#include "../primitives/quad.h"
#include "../primitives/volume.h"

#include "../lights/directional_light.h"
#include "../lights/omni_light.h"
#include "../lights/spot_light.h"



std::shared_ptr<hittable> scene_factory::createBox(
        const std::string name,
        const point3 &p0,
        const point3 &p1,
        const std::shared_ptr<material> &material,
        const uvmapping& uv)
{
    return std::make_shared<box>(p0, p1, material, uv, name);
}

std::shared_ptr<hittable> scene_factory::createCylinder(
        const std::string name,
        const point3 &center,
        double radius,
        double height,
        const std::shared_ptr<material> &material,
        const uvmapping& uv)
{
    return std::make_shared<cylinder>(center, radius, height, material, uv);
}

std::shared_ptr<hittable> scene_factory::createDisk(
        const std::string name,
        const point3& center,
        double radius,
        double height,
        const std::shared_ptr<material>& material,
        const uvmapping& uv)
{
    return std::make_shared<disk>(center, radius, height, material, uv);
}

std::shared_ptr<hittable> scene_factory::createTorus(
        const std::string name,
        const point3& center,
        double major_radius,
        double minor_radius,
        const std::shared_ptr<material>& material,
        const uvmapping& uv)
{
    return std::make_shared<torus>(center, major_radius, minor_radius, material, uv);
}

std::shared_ptr<hittable> scene_factory::createSphere(
        const std::string name,
        const point3& center,
        double radius,
        const std::shared_ptr<material> &material,
        const uvmapping& uv)
{
    return std::make_shared<sphere>(center, radius, material, uv, name);
}

std::shared_ptr<hittable> scene_factory::createCone(
        const std::string name,
        const point3& center,
        double height,
        double radius,
        const std::shared_ptr<material> &material,
        const uvmapping& uv)
{
    return std::make_shared<cone>(center, radius, height, material, uv, name);
}

std::shared_ptr<hittable> scene_factory::createPlane(
    const std::string name,
    const point3& p0,
    const point3& p1,
    const std::shared_ptr<material> &material,
    const uvmapping& uv)
{
    if (p0.x == p1.x)
    {
        double x = p0.x;
        double y0 = p0.y < p1.y ? p0.y : p1.y;
        double y1 = p0.y < p1.y ? p1.y : p0.y;
        double z0 = p0.z < p1.z ? p0.z : p1.z;
        double z1 = p0.z < p1.z ? p1.z : p0.z;

        return std::make_shared<yz_rect>(y0, y1, z0, z1, x, material, uv);
    }

    if (p0.y == p1.y)
    {
        double y = p0.y;
        double x0 = p0.x < p1.x ? p0.x : p1.x;
        double x1 = p0.x < p1.x ? p1.x : p0.x;
        double z0 = p0.z < p1.z ? p0.z : p1.z;
        double z1 = p0.z < p1.z ? p1.z : p0.z;

        return std::make_shared<xz_rect>(x0, x1, z0, z1, y, material, uv);
    }

    if (p0.z == p1.z)
    {
        double z = p0.z;
        double x0 = p0.x < p1.x ? p0.x : p1.x;
        double x1 = p0.x < p1.x ? p1.x : p0.x;
        double y0 = p0.y < p1.y ? p0.y : p1.y;
        double y1 = p0.y < p1.y ? p1.y : p0.y;

        return std::make_shared<xy_rect>(x0, x1, y0, y1, z, material, uv);
    }

    throw std::runtime_error("a plane should always be created aligned to one of the x, y, or z axes");
}

std::shared_ptr<hittable> scene_factory::createQuad(
    const std::string name,
    const point3& position,
    const vector3 u,
    const vector3 v,
    const std::shared_ptr<material>& material,
    const uvmapping& uv)
{
    return std::make_shared<quad>(position, u, v, material, uv, name);
}

std::shared_ptr<hittable> scene_factory::createVolume(
    const std::string name,
    const std::shared_ptr<hittable>& boundary,
    double density,
    std::shared_ptr<texture> texture)
{
    return std::make_shared<volume>(boundary, density, texture, name);
}

std::shared_ptr<hittable> scene_factory::createVolume(
    const std::string name,
    const std::shared_ptr<hittable>& boundary,
    double density,
    const color& rgb)
{
    return std::make_shared<volume>(boundary, density, rgb, name);
}

std::shared_ptr<hittable> scene_factory::createObjMesh(
	const std::string name,
	const point3& center,
	const std::string filepath,
	const std::shared_ptr<material>& material,
	const bool use_mtl,
    const bool use_smoothing,
    randomizer& rnd)
{
    std::shared_ptr<hittable> mesh = nullptr;
    
    obj_mesh_loader::obj_mesh_data data;
    
    if (obj_mesh_loader::load_model_from_file(filepath, data))
    {
        mesh = obj_mesh_loader::convert_model_from_file(data, material, use_mtl, use_smoothing, rnd, name);
    }

    return mesh;
}

std::shared_ptr<hittable> scene_factory::createFbxMesh(
    const std::string name,
    const point3& center,
    const std::string filepath,
    bool use_fbx_cameras,
    bool use_fbx_lights,
    bool use_fbx_materials,
    bool use_fbx_textures,
	std::vector<std::shared_ptr<camera>>& cameras,
    std::vector<std::shared_ptr<light>>& lights,
    double aspect_ratio,
    randomizer& rnd,
    const std::map<std::string, std::shared_ptr<material>>& scene_materials,
    const std::map<std::string, std::shared_ptr<texture>>& scene_textures)
{
    std::shared_ptr<hittable> meshes = nullptr;

    fbx_mesh_loader::fbx_mesh_data data;

    if (fbx_mesh_loader::load_model_from_file(filepath, data))
    {
        meshes = fbx_mesh_loader::get_meshes(data, rnd, scene_materials, scene_textures, use_fbx_materials, use_fbx_textures, name);

        if (use_fbx_cameras)
            cameras = fbx_mesh_loader::get_cameras(data, aspect_ratio);
        else
            std::cout << "[INFO] Not using FBX camera" << std::endl;

        if (use_fbx_lights)
            lights = fbx_mesh_loader::get_lights(data);
        else
            std::cout << "[INFO] Not using FBX lights" << std::endl;
    }

    return meshes;
}

std::shared_ptr<hittable> scene_factory::createDirectionalLight(std::string name, const point3& pos, const vector3& u, const vector3& v, double intensity, color rgb, bool invisible)
{
    return std::make_shared<directional_light>(pos, u, v, intensity, rgb, name, invisible);
}

std::shared_ptr<hittable> scene_factory::createOmniDirectionalLight(std::string name, const point3& pos, double radius, double intensity, color rgb, bool invisible)
{
    return std::make_shared<omni_light>(pos, radius, intensity, rgb, name, invisible);
}

std::shared_ptr<hittable> scene_factory::createSpotLight(std::string name, const point3& pos, const vector3& dir, double cutoff, double falloff, double intensity, double radius, color rgb, bool invisible)
{
    return std::make_shared<spot_light>(pos, dir, cutoff, falloff, intensity, radius, rgb, name, invisible);
}

