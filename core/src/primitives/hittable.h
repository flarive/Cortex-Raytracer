#ifndef HITTABLE_H
#define HITTABLE_H

#include "../misc/ray.h"
#include "../interval.h"
#include "../aabb.h"

class material;

class hit_record
{
public:
    point3 hit_point; // point (coordinates) where the hit occurred
    vector3 normal; // normal vector where the hit occurred
    shared_ptr<material> mat; // material of the object hit by the ray
    double t;
    double u;
    double v;
    bool front_face; // front-face tracking (object was hit from outside (frontface) or inside (backface) ?)
    string name; // name of the object that was hit
    aabb bbox; // bounding box size of the object that was hit

    void set_face_normal(const ray& r, const vector3& outward_normal)
    {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        if (glm::dot(r.direction(), outward_normal) > 0.0)
        {
            // ray is inside the hittable primitive
            normal = -outward_normal;
            front_face = false;
        }
        else
        {
            // ray is outside the hittable primitive
            normal = outward_normal;
            front_face = true;
        }
    }
};

/// <summary>
/// Base class for all primitives that can be hit by a ray
/// </summary>
class hittable
{
public:
    virtual ~hittable() = default;

    // virtual hit method that needs to be implemented for all primitives
    // because each primitive has it own intersection calculation logic
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const = 0;

    virtual aabb bounding_box() const = 0;

    virtual double pdf_value(const point3& o, const vector3& v) const
    {
        return 0.0;
    }

    virtual vector3 random(const vector3& o) const
    {
        return vector3(1, 0, 0);
    }

    void setName(string _name)
    {
        name = _name;
    }

    string getName() const
    {
        return(name);
    }


protected:
    string name = "Hitable";

private:
    virtual void updateBoundingBox() = 0;
};

class flip_normals : public hittable
{
public:
    flip_normals(shared_ptr<hittable> p) : object(p)
    {
        bbox = object->bounding_box();
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override
    {
        if (object->hit(r, ray_t, rec, depth))
        {
            rec.normal = -rec.normal;
            return true;
        }
        else
            return false;
    }


    aabb bounding_box() const override
    {
        return bbox;
    }

private:
    shared_ptr<hittable> object;
    aabb bbox;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override
    {
        // to implement
    }
};


/// <summary>
/// Translate an instance
/// </summary>
class translate : public hittable
{
public:

    translate(shared_ptr<hittable> p, const vector3& displacement)
        : object(p), offset(displacement)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override
    {
        // Move the ray backwards by the offset
        ray offset_r(r.origin() - offset, r.direction(), r.time());

        // Determine where (if any) an intersection occurs along the offset ray
        if (!object->hit(offset_r, ray_t, rec, depth))
            return false;

        // Move the intersection point forwards by the offset
        rec.hit_point += offset;

        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

private:
    shared_ptr<hittable> object;
    vector3 offset;
    aabb bbox;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override
    {
        // to implement
    }
};


/// <summary>
/// Rotate an instance
/// </summary>
class rotate_y : public hittable
{
public:

    rotate_y(shared_ptr<hittable> p, double angle) : object(p)
    {
        auto radians = degrees_to_radians(angle);
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        bbox = object->bounding_box();

        point3 min(infinity, infinity, infinity);
        point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    auto newx = cos_theta * x + sin_theta * z;
                    auto newz = -sin_theta * x + cos_theta * z;

                    vector3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = aabb(min, max);
    }

    
    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override
    {
        // Change the ray from world space to object space
        auto origin = r.origin();
        auto direction = r.direction();

        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

        direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
        direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];

        ray rotated_r(origin, direction, r.time());

        // Determine where (if any) an intersection occurs in object space
        if (!object->hit(rotated_r, ray_t, rec, depth))
            return false;

        // Change the intersection point from object space to world space
        auto p = rec.hit_point;
        p[0] = cos_theta * rec.hit_point[0] + sin_theta * rec.hit_point[2];
        p[2] = -sin_theta * rec.hit_point[0] + cos_theta * rec.hit_point[2];

        // Change the normal from object space to world space
        auto normal = rec.normal;
        normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
        normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

        rec.hit_point = p;
        rec.normal = normal;

        return true;
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override
    {
        // to implement
    }
};

#endif