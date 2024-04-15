#include "cone.h"

cone::cone() : center(vector3(0)), radius(1), height(1)
{
    name = "Cone";

    // calculate cone bounding box for ray optimizations
    bbox = aabb(
        point3(center.x - radius, center.y, center.z - radius),
        point3(center.x + radius, center.y + height, center.z + radius)
    );
};

cone::cone(vector3 _center, double _radius, double _height, std::shared_ptr<material> _material, std::string _name)
    : center(_center), radius(_radius), height(_height), mat(_material)
{
    name = _name;

    // calculate cone bounding box for ray optimizations
    bbox = aabb(
        point3(center.x - radius, center.y, center.z - radius),
        point3(center.x + radius, center.y + height, center.z + radius)
    );
};

bool cone::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    double A = r.origin().x - center.x;
    double B = r.origin().z - center.z;
    double D = height - r.origin().y + center.y;

    double tan = (radius / height) * (radius / height);

    double a = (r.direction().x * r.direction().x) + (r.direction().z * r.direction().z) - (tan * (r.direction().y * r.direction().y));
    double b = (2 * A * r.direction().x) + (2 * B * r.direction().z) + (2 * tan * D * r.direction().y);
    double c = (A * A) + (B * B) - (tan * (D * D));

    double delta = b * b - 4 * (a * c);
    if (fabs(delta) < 0.001) return false;

    if (delta < 0)
    {
        return false;
    }

    double root = (-b - sqrt(delta)) / (2 * a);

    if (root < ray_t.min || ray_t.max < root)
    {
        root = (-b + sqrt(delta)) / (2 * a);
        if (root < ray_t.min || ray_t.max < root)
        {
            return false;
        }
    }


    double y = r.origin().y + root * r.direction()[1];

    if ((y < center.y) || (y > center.y + height))
    {
        return false;
    }

    double rs = sqrt((rec.hit_point.x - center.x) * (rec.hit_point.x - center.x) + (rec.hit_point.z - center.z) * (rec.hit_point.z - center.z));
    vector3 n = vector3(rec.hit_point.x - center.x, rs * (radius / height), rec.hit_point.z - center.z);


    rec.t = root;
    rec.hit_point = r.at(rec.t);
    rec.normal = glm::normalize(n);
    vector3 outward_normal = (rec.hit_point - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_cone_uv(outward_normal, rec.u, rec.v, radius);
    rec.mat = mat;
    rec.name = name;
    rec.bbox = bbox;

    return true;
}

aabb cone::bounding_box() const
{
    return bbox;
}

void cone::get_cone_uv(const vector3& p, double& u, double& v, double radius) const
{
    /*auto theta = std::atan2(p.x, p.z);
    auto phi = std::atan2(p.y, radius);
    u = 1 - (theta + M_PI) / (2 * M_PI);
    v = (phi + M_PI / 2) / M_PI;*/

    auto theta = std::atan2(p.z, p.x);
    auto phi = std::atan2(p.y, radius);
    u = 1 - (theta + M_PI) / (2 * M_PI);
    v = phi / (2 * M_PI); // Linear mapping of phi to [0, 1]
}


/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void cone::updateBoundingBox()
{
    // to implement
}