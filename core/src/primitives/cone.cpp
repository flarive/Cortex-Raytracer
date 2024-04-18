#include "cone.h"

#include "../utilities/uvmapping.h"

cone::cone(std::string _name) : center(vector3(0)), radius(1), height(1)
{
    cone guard(center, radius, height, nullptr, _name);
};

cone::cone(vector3 _center, double _radius, double _height, std::shared_ptr<material> _material, std::string _name)
    : center(_center), radius(_radius), height(_height), mat(_material)
{
    cone guard(_center, _radius, _height, _material, _name);
};


cone::cone(vector3 _center, double _radius, double _height, std::shared_ptr<material> _material, const uvmapping& _mapping, std::string _name)
    : center(_center), radius(_radius), height(_height), mat(_material)
{
    m_mapping = _mapping;
    name = _name;

    // calculate cone bounding box for ray optimizations
    bbox = aabb(
        vector3(center.x - radius, center.y, center.z - radius),
        vector3(center.x + radius, center.y + height, center.z + radius)
    );
}

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

    //double rs = sqrt((rec.hit_point.x - center.x) * (rec.hit_point.x - center.x) + (rec.hit_point.z - center.z) * (rec.hit_point.z - center.z));
    //vector3 n = vector3(rec.hit_point.x - center.x, rs * (radius / height), rec.hit_point.z - center.z);


    rec.t = root;
    rec.hit_point = r.at(rec.t);
    //rec.normal = glm::normalize(n);
    /*vector3 outward_normal = (rec.hit_point - center) / radius;
    rec.set_face_normal(r, outward_normal);*/


	// Calculate the outward normal
	vector3 outward_normal;
	if (rec.hit_point.y <= center.y) {
		// Point lies on the base of the cone
		outward_normal = vector3(0, -1, 0);
	}
	else if (rec.hit_point.y >= center.y + height) {
		// Point lies on the top of the cone
		outward_normal = vector3(0, 1, 0);
	}
	else {
		// Point lies on the curved surface of the cone
		double rs = sqrt((rec.hit_point.x - center.x) * (rec.hit_point.x - center.x) + (rec.hit_point.z - center.z) * (rec.hit_point.z - center.z));
		outward_normal = vector3(rec.hit_point.x - center.x, rs * (radius / height), rec.hit_point.z - center.z);
	}

	// Normalize the outward normal
	//rec.normal = glm::normalize(outward_normal);

    rec.set_face_normal(r, glm::normalize(outward_normal));



    get_cone_uv(outward_normal, rec.u, rec.v, radius, height, m_mapping);
    rec.mat = mat;
    rec.name = name;
    rec.bbox = bbox;

    return true;
}

aabb cone::bounding_box() const
{
    return bbox;
}



/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void cone::updateBoundingBox()
{
    // to implement
}