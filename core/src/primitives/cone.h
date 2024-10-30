#pragma once

#include "../constants.h"
#include "hittable.h"
#include "hittable_list.h"
#include "../misc/ray.h"
#include "../misc/aabb.h"
#include "../utilities/interval.h"
#include "../utilities/uvmapping.h"
#include "../misc/hit_record.h"

/// <summary>
/// Cone primitive
/// https://github.com/iceman201/RayTracing/blob/master/Ray%20tracing/Cone.cpp
/// </summary>
class cone : public hittable
{
public:
    cone(std::string _name = "Cone");
    cone(vector3 _center, double _radius, double _height, std::shared_ptr<material> _material, std::string _name = "Cone");
    cone(vector3 _center, double _radius, double _height, std::shared_ptr<material> _material, const uvmapping& _mapping, std::string _name = "Cone");
	virtual ~cone() {}

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const;
    aabb bounding_box() const override;

private:
    vector3 center;
    double radius;
    double height;
    std::shared_ptr<material> mat;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;
};

