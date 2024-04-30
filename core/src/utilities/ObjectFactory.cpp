#include "ObjectFactory.h"


#include "../primitives/box.h"
#include "../primitives/cone.h"
#include "../primitives/sphere.h"
#include "../primitives/cylinder.h"
#include "../primitives/disk.h"
#include "../primitives/torus.h"
#include "../primitives/aarect.h"

#include "../lights/quad_light.h"
#include "../lights/sphere_light.h"

std::shared_ptr<hittable> ObjectFactory::createBox(
        const std::string name,
        const point3 &p0,
        const point3 &p1,
        const std::shared_ptr<material> &material,
        const uvmapping& uv)
{
    return std::make_shared<box>(p0, p1, material, uv, name);
}

std::shared_ptr<hittable> ObjectFactory::createCylinder(
        const std::string name,
        const point3 &center,
        double radius,
        double height,
        const std::shared_ptr<material> &material,
        const uvmapping& uv)
{
    return std::make_shared<cylinder>(center, radius, height, material, uv);
}

std::shared_ptr<hittable> ObjectFactory::createDisk(
        const std::string name,
        const point3& center,
        double radius,
        double height,
        const std::shared_ptr<material>& material,
        const uvmapping& uv)
{
    return std::make_shared<disk>(center, radius, height, material, uv);
}

std::shared_ptr<hittable> ObjectFactory::createTorus(
        const std::string name,
        const point3& center,
        double major_radius,
        double minor_radius,
        const std::shared_ptr<material>& material,
        const uvmapping& uv)
{
    return std::make_shared<torus>(center, major_radius, minor_radius, material, uv);
}

std::shared_ptr<hittable> ObjectFactory::createSphere(
        const std::string name,
        const point3& center,
        double radius,
        const std::shared_ptr<material> &material,
        const uvmapping& uv)
{
    return std::make_shared<sphere>(center, radius, material, uv, name);
}

std::shared_ptr<hittable> ObjectFactory::createCone(
        const std::string name,
        const point3& center,
        double height,
        double radius,
        const std::shared_ptr<material> &material,
        const uvmapping& uv)
{
    return std::make_shared<cone>(center, radius, height, material, uv, name);
}

std::shared_ptr<hittable> ObjectFactory::createPlane(
    const std::string name,
    const point3 &p0,
    point3 p1,
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

std::shared_ptr<hittable> ObjectFactory::createDirectionalLight(std::string name, const point3& pos, const vector3& u, const vector3& v, double intensity, color rgb, bool invisible)
{
    return std::make_shared<quad_light>(pos, u, v, intensity, rgb, name);
}

std::shared_ptr<hittable> ObjectFactory::createOmniDirectionalLight(std::string name, const point3& pos, double radius, double intensity, color rgb, bool invisible)
{
    return std::make_shared<sphere_light>(pos, radius, intensity, rgb, name);
}
