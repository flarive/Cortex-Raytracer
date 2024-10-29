#include "triangle.h"

#include "../misc/singleton.h"
#include "../utilities/triangle_cache.h"

#include <glm/glm.hpp>

#define EPS 0.000001

triangle::triangle(std::string _name)
{
}

triangle::triangle(size_t shape_index, size_t triangle_index, const vector3 v0, const vector3 v1, const vector3 v2, std::shared_ptr<material> m, std::string _name)
    : triangle(shape_index, triangle_index, v0, v1, v2, vector3(), vector3(), vector3(), vector2(), vector2(), vector2(), vector3(), vector3(), vector3(), vector3(), vector3(), vector3(), false, m, _name)
{
}

triangle::triangle(size_t shape_index, size_t triangle_index, const vector3 v0, const vector3 v1, const vector3 v2, const vector3 vn0, const vector3 vn1, const vector3 vn2, bool smooth_shading, std::shared_ptr<material> m, std::string _name)
    : triangle(shape_index, triangle_index, v0, v1, v2, vn0, vn1, vn2, vector2(), vector2(), vector2(), vector3(), vector3(), vector3(), vector3(), vector3(), vector3(), false, m, _name)
{
}

triangle::triangle(size_t shape_index, size_t triangle_index, const vector3 v0, const vector3 v1, const vector3 v2, const vector3 vn0, const vector3 vn1, const vector3 vn2, const vector2& vuv0, const vector2& vuv1, const vector2& vuv2, bool smooth_shading, std::shared_ptr<material> m, std::string _name)
	: triangle(shape_index, triangle_index, v0, v1, v2, vn0, vn1, vn2, vuv0, vuv1, vuv2, vector3(), vector3(), vector3(), vector3(), vector3(), vector3(), false, m, _name)
{
}

triangle::triangle(size_t shape_index, size_t triangle_index, const vector3 v0, const vector3 v1, const vector3 v2, const vector3 vn0, const vector3 vn1, const vector3 vn2, const vector2& vuv0, const vector2& vuv1, const vector2& vuv2, const vector3& tan0, const vector3& tan1, const vector3& tan2,
	const vector3& bitan0, const vector3& bitan1, const vector3& bitan2, bool smooth_shading, std::shared_ptr<material> m, std::string _name) : mat_ptr(m)
{
    verts[0] = v0;
    verts[1] = v1;
    verts[2] = v2;

    vert_normals[0] = unit_vector(vn0);
    vert_normals[1] = unit_vector(vn1);
    vert_normals[2] = unit_vector(vn2);

    vert_uvs[0] = vuv0;
    vert_uvs[1] = vuv1;
    vert_uvs[2] = vuv2;

    vert_tangents[0] = tan0;
    vert_tangents[1] = tan1;
    vert_tangents[2] = tan2;

	vert_bitangents[0] = bitan0;
    vert_bitangents[1] = bitan1;
    vert_bitangents[2] = bitan2;

    smooth_normals = smooth_shading;

    v0_v1 = verts[1] - verts[0];
    v0_v2 = verts[2] - verts[0];

    //double a = (v0 - v1).length();
    //double b = (v1 - v2).length();
    //double c = (v2 - v0).length();
    //double s = (a + b + c) / 2.0;
    //area = sqrt(fabs(s * (s - a) * (s - b) * (s - c)));
    //middle_normal = unit_vector(glm::cross(v0 - v1, v0 - v2));


    auto [cached_area, cached_middle_normal] = TriangleCache::get_area_and_normal(shape_index, triangle_index, v0, v1, v2);
    area = cached_area;
    middle_normal = cached_middle_normal;


    // bounding box
    vector3 max_extent = max(max(verts[0], verts[1]), verts[2]);
    vector3 min_extent = min(min(verts[0], verts[1]), verts[2]);
    double eps = 0.001;
    auto epsv = vector3(eps, eps, eps);
    m_bbox = aabb(min_extent - epsv, max_extent + epsv);
}

bool triangle::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    // Möller-Trumbore algorithm for fast triangle hit
    // https://web.archive.org/web/20200927071045/https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection

    auto parallel_vec = glm::cross(r.direction(), v0_v2);
    auto det = glm::dot(v0_v1, parallel_vec);
    // If det < 0, this is a back-facing intersection, change hit_record front_face
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < EPS) return false;
    auto inv_det = 1.0 / det;

    auto tvec = r.origin() - verts[0];
    auto u = glm::dot(tvec, parallel_vec) * inv_det;
    if (u < 0 || u > 1) return false;

    auto qvec = glm::cross(tvec, v0_v1);
    auto v = glm::dot(r.direction(), qvec) * inv_det;
    if (v < 0 || u + v > 1) return false;

    double t = glm::dot(v0_v2, qvec) * inv_det;
    if (t < ray_t.min || t > ray_t.max) return false;

    rec.t = t;

    // UV coordinates
    get_triangle_uv(rec.hit_point, rec.u, rec.v, verts, vert_uvs);

    rec.hit_point = r.at(t);
    rec.mat = mat_ptr;

    vector3 normal;
    
    if (smooth_normals)
    {
        double a = u, b = v, c = 1 - u - v;
        // What does u and v map to?
        normal = a * vert_normals[1] + b * vert_normals[2] + c * vert_normals[0];
    }
    else
    {
        normal = middle_normal;
    }

    // set normal and front-face tracking
    vector3 outward_normal = normal;
    rec.set_face_normal(r, outward_normal);

    // no need to calculate tangents and bitangents, just get them from obj file
    rec.tangent = vert_tangents[0];
    rec.bitangent = vert_bitangents[0];

    return true;
}

aabb triangle::bounding_box() const
{
    return m_bbox;
}

double triangle::pdf_value(const point3& o, const vector3& v) const
{
    hit_record rec;
    if (!this->hit(ray(o, v), interval(EPS, infinity), rec, 0))
        return 0;

    vector3 R1 = verts[0] - o;
    vector3 R2 = verts[1] - o;
    vector3 R3 = verts[2] - o;

    double r1_sq = glm::dot(R1, R1);
    double r2_sq = glm::dot(R2, R2);
    double r3_sq = glm::dot(R3, R3);

    double N = glm::dot(R1, cross(R2, R3));

    // Precompute dot products once
    double dotR1R2 = glm::dot(R1, R2);
    double dotR1R3 = glm::dot(R1, R3);
    double dotR2R3 = glm::dot(R2, R3);

    // Compute D with optimized terms
    double D = std::sqrt(r1_sq * r2_sq * r3_sq) + dotR1R2 * std::sqrt(r3_sq) + dotR1R3 * std::sqrt(r2_sq) + dotR2R3 * std::sqrt(r1_sq);

    double omega = util::atan2(N, D);

    return 1.0 / omega;
}

//vector3 triangle::random(const point3& o, randomizer& rnd) const
//{
//    // From https://math.stackexchange.com/questions/18686/uniform-random-point-in-triangle-in-3d
//    double r1 = rnd.get_real(0.0, 1.0);
//    double r2 = rnd.get_real(0.0, 1.0);
//    double ca = (1.0 - glm::sqrt(r1)), cb = glm::sqrt(r1) * (1. - r2), cc = r2 * glm::sqrt(r1);
//    vector3 random_in_triangle = verts[0] * ca + verts[1] * cb + verts[2] * cc;
//    return random_in_triangle - o;
//}

vector3 triangle::random(const point3& o, randomizer& rnd) const
{
    // From https://math.stackexchange.com/questions/18686/uniform-random-point-in-triangle-in-3d
    // Generate two random values between 0 and 1
    double r1 = rnd.get_real(0.0, 1.0);
    double r2 = rnd.get_real(0.0, 1.0);

    // Calculate sqrt of r1 only once
    double sqrt_r1 = glm::sqrt(r1);

    // Calculate the weights for each vertex
    double ca = 1.0 - sqrt_r1;
    double cb = sqrt_r1 * (1.0 - r2);
    double cc = sqrt_r1 * r2;

    // Calculate and return the weighted sum, adjusted by point `o`
    return verts[0] * ca + verts[1] * cb + verts[2] * cc - o;
}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void triangle::updateBoundingBox()
{
    // to implement
}




