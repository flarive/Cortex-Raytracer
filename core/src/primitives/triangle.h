#pragma once

#include "../constants.h"
#include "../utilities/types.h"
#include "../utilities/math_utils.h"
#include "../primitives/hittable.h"
#include "../aabb.h"
#include "../misc/hit_record.h"

/// <summary>
/// Triangle primitive
/// https://github.com/Drummersbrother/raytracing-in-one-weekend/blob/90b1d3d7ce7f6f9244bcb925c77baed4e9d51705/triangle.h
/// </summary>
class triangle: public hittable
{
    public:
        triangle();
        triangle(const vector3 v0, const vector3 v1, const vector3 v2, std::shared_ptr<material> m);
        triangle(const vector3 v0, const vector3 v1, const vector3 v2, const vector3 vn0, const vector3 vn1, const vector3 vn2, bool smooth_shading, std::shared_ptr<material> m);

        triangle(const vector3 v0, const vector3 v1, const vector3 v2, const vector3 vn0, const vector3 vn1, const vector3 vn2, const vector2& vuv0, const vector2& vuv1, const vector2& vuv2, bool smooth_shading, std::shared_ptr<material> m);

 

        virtual bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;

        virtual aabb bounding_box() const override;

        double pdf_value(const point3& o, const vector3& v) const override;


        /// <summary>
        /// Random special implementation for sphere (override base)
        /// </summary>
        /// <param name="origin"></param>
        /// <returns></returns>
        vector3 random(const point3& o) const override;

    public:
        vector3 verts[3];
        std::shared_ptr<material> mat_ptr;
        vector3 vert_normals[3];
        bool smooth_normals;
        vector2 vert_uvs[3];
    private:
        double area;
        vector3 middle_normal;


        /// <summary>
        /// Update the internal AABB of the mesh.
        /// Warning: run this when the mesh is updated.
        /// </summary>
        void updateBoundingBox() override;

        static vector2 calculateTextureCoordinate(vector2 uv0, vector2 uv1, vector2 uv2, const vector2& barycentricCoords);
};