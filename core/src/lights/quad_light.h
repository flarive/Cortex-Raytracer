#ifndef QUAD_LIGHT_H
#define QUAD_LIGHT_H

#include "light.h"
#include "../primitives/hittable.h"
#include "../misc/color.h"
#include "../materials/diffuse_light.h"
#include "../utilities/math_utils.h"
#include "renderParameters.h"
#include "singleton.h"


/// <summary>
/// Quad light
/// </summary>
class quad_light : public light
{
public:
    quad_light(const point3& _Q, const vector3& _u, const vector3& _v, double _intensity, color _color, string _name = "QuadLight", bool _invisible = true)
        : Q(_Q), u(_u), v(_v)
    {
        intensity = _intensity;
        c = _color * _intensity;
        invisible = _invisible;

        name = _name;

        mat = std::make_shared<diffuse_light>(c, true, invisible);
        
        auto n = glm::cross(u, v);
        normal = unit_vector(n);
        D = glm::dot(normal, Q);
        w = n / glm::dot(n, n);

        area = vector_length(n);

        set_bounding_box();
    }
    
    void set_bounding_box()
    {
        bbox = aabb(Q, Q + u + v).pad();
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override
    {
        auto denom = glm::dot(normal, r.direction());

        // No hit if the ray is parallel to the plane.
        if (fabs(denom) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        double t = (D - glm::dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        // Determine the hit point lies within the planar shape using its plane coordinates.
        auto intersection = r.at(t);
        vector3 planar_hitpt_vector = intersection - Q;
        auto alpha = glm::dot(w, glm::cross(planar_hitpt_vector, v));
        auto beta = glm::dot(w, glm::cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
            return false;

        
        Singleton* singleton = Singleton::getInstance();
        if (singleton)
        {
            auto renderParams = singleton->value();
			if (invisible && depth == renderParams.recursionMaxDepth)
			{
				return false;
			}
        }


        

        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.name = name;
        rec.bbox = bbox;
        rec.set_face_normal(r, normal);

        return true;
    }

    bool is_interior(double a, double b, hit_record& rec) const
    {
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true.

        if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

    double pdf_value(const point3& origin, const vector3& v) const override
    {
        hit_record rec;

        if (!this->hit(ray(origin, v), interval(0.001, infinity), rec, 0))
            return 0;

        auto distance_squared = rec.t * rec.t * vector_length_squared(v);
        auto cosine = fabs(dot(v, rec.normal) / vector_length(v));

        return distance_squared / (cosine * area);
    }

    /// <summary>
    /// Random special implementation for quad light (override base)
    /// </summary>
    /// <param name="origin"></param>
    /// <returns></returns>
    vector3 random(const point3& origin) const override
    {
        auto p = Q + (random_double() * u) + (random_double() * v);
        return p - origin;
    }

private:
    point3 Q; // the lower-left corner
    vector3 u; // a vector representing the first side
    vector3 v; //  a vector representing the second side
    vector3 normal;
    double D;
    vector3 w; // The vector w is constant for a given quadrilateral, so we'll cache that value
    double area;
};

#endif