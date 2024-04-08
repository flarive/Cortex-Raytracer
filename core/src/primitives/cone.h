#pragma once

#include "../constants.h"
#include "hittable.h"
#include "hittable_list.h"
#include "../misc/ray.h"
#include "../aabb.h"
#include "../utilities/interval.h"
#include "../misc/hit_record.h"

/// <summary>
/// Cone primitive
/// https://github.com/iceman201/RayTracing/blob/master/Ray%20tracing/Cone.cpp
/// </summary>
class cone : public hittable
{
public:
    cone();
    cone(vector3 _center, double _radius, double _height, std::shared_ptr<material> _material, std::string _name = "Cone");
	virtual ~cone() {}

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const;
    aabb bounding_box() const override;

private:
    vector3 center;
    double radius;
    double height;
    std::shared_ptr<material> mat;
    aabb bbox; // bounding box

    void get_cone_uv(const vector3& p, double& u, double& v, double radius) const;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;
};

