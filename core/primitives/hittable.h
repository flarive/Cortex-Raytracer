#pragma once

#include "../misc/aabb.h"
#include "../misc/ray.h"
#include "../utilities/interval.h"
#include "../utilities/uvmapping.h"
#include "../utilities/types.h"
#include "../randomizers/randomizer.h"

#include <string>


class material;
class hit_record;

/// <summary>
/// Base class for all primitives that can be hit by a ray
/// </summary>
class hittable
{
public:
    virtual ~hittable() = default;

    // pure virtual function 
    // virtual hit method that needs to be implemented for all primitives
    // because each primitive has it own intersection calculation logic
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec, int depth, randomizer& rnd) const = 0;

    virtual aabb bounding_box() const = 0;

    virtual double pdf_value(const point3& o, const vector3& v, randomizer& rnd) const;


    virtual vector3 random(const vector3& o, randomizer& rnd) const;


    void setName(std::string _name);
    std::string getName() const;



protected:
    aabb m_bbox{};
    uvmapping m_mapping{};
    std::string m_name = "Hittable";

private:
    virtual void updateBoundingBox() = 0;
};