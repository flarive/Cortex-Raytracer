#include "ObjectFactory.h"


#include "../primitives/box.h"
#include "../primitives/cone.h"
#include "../primitives/sphere.h"
#include "../primitives/cylinder.h"
#include "../primitives/aarect.h"
//#include "PlaneYZ.hpp"
//#include "PlaneXZ.hpp"
//#include "PlaneXY.hpp"



std::shared_ptr<hittable> ObjectFactory::createBox(
        const point3 &p0,
        const point3 &p1,
        const std::shared_ptr<material> &material)
{
    return std::make_shared<
            box
    >(p0, p1, material);
}

std::shared_ptr<hittable> ObjectFactory::createCylinder(
        const point3 &center,
        double radius,
        double height,
        const std::shared_ptr<material> &material)
{
    return std::make_shared<
            cylinder
    >(center, radius, height, material);
}

std::shared_ptr<hittable> ObjectFactory::createSphere(
        const point3 &center,
        double radius,
        const std::shared_ptr<material> &material)
{
    return std::make_shared<
            sphere
    >(center, radius, material);
}

std::shared_ptr<hittable> ObjectFactory::createCone(
        const point3& center,
        double height,
        double radius,
        const std::shared_ptr<material> &material)
{
    return std::make_shared<
            cone
    >(vector3(center.x, center.y, center.z), height, radius, material);
}


std::shared_ptr<hittable> ObjectFactory::createPlane(
        const point3 &p0,
        point3 p1,
        const std::shared_ptr<material> &material)
{
    if (p0.x == p1.x)
    {
        double x = p0.x;
        double y0 = p0.y < p1.y ? p0.y : p1.y;
        double y1 = p0.y < p1.y ? p1.y : p0.y;
        double z0 = p0.z < p1.z ? p0.z : p1.z;
        double z1 = p0.z < p1.z ? p1.z : p0.z;

        return std::make_shared<
            yz_rect
        >(y0, y1, z0, z1, x, material);
    }

    if (p0.y == p1.y)
    {
        double y = p0.y;
        double x0 = p0.x < p1.x ? p0.x : p1.x;
        double x1 = p0.x < p1.x ? p1.x : p0.x;
        double z0 = p0.z < p1.z ? p0.z : p1.z;
        double z1 = p0.z < p1.z ? p1.z : p0.z;

        return std::make_shared<
            xz_rect
        >(x0, x1, z0, z1, y, material);
    }

    if (p0.z == p1.z)
    {
        double z = p0.z;
        double x0 = p0.x < p1.x ? p0.x : p1.x;
        double x1 = p0.x < p1.x ? p1.x : p0.x;
        double y0 = p0.y < p1.y ? p0.y : p1.y;
        double y1 = p0.y < p1.y ? p1.y : p0.y;

        return std::make_shared<
            xy_rect
        >(x0, x1, y0, y1, z, material);
    }

    throw std::runtime_error("a plane should always be created aligned to one of the x, y, or z axes");
}
