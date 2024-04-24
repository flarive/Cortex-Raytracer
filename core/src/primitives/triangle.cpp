#include "triangle.h"

#define EPS 0.000001

triangle::triangle()
{
}

triangle::triangle(const vector3 v0, const vector3 v1, const vector3 v2, std::shared_ptr<material> m) 
    : triangle(v0, v1, v2, vector3(), vector3(), vector3(), vector2(), vector2(), vector2(), false, m)
{
}

triangle::triangle(const vector3 v0, const vector3 v1, const vector3 v2, const vector3 vn0, const vector3 vn1, const vector3 vn2, bool smooth_shading, std::shared_ptr<material> m)
    : triangle(v0, v1, v2, vector3(), vector3(), vector3(), vector2(), vector2(), vector2(), false, m)
{
}


triangle::triangle(const vector3 v0, const vector3 v1, const vector3 v2, const vector3 vn0, const vector3 vn1, const vector3 vn2, const vector2& vuv0, const vector2& vuv1, const vector2& vuv2, bool smooth_shading, std::shared_ptr<material> m) : mat_ptr(m)
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

    smooth_normals = smooth_shading;

    double a = (v0 - v1).length();
    double b = (v1 - v2).length();
    double c = (v2 - v0).length();
    double s = (a + b + c) / 2.; area = sqrt(fabs(s * (s - a) * (s - b) * (s - c)));
    middle_normal = unit_vector(cross(v0 - v1, v0 - v2));

    // bounding box
    vector3 max_extent = max(max(verts[0], verts[1]), verts[2]);
    vector3 min_extent = min(min(verts[0], verts[1]), verts[2]);
    double eps = 0.001; auto epsv = vector3(eps, eps, eps);
    bbox = aabb(min_extent - epsv, max_extent + epsv);
}

bool triangle::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
{
    // Möller-Trumbore algorithm for fast triangle hit
    // https://web.archive.org/web/20200927071045/https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
    auto v0_v1 = verts[1] - verts[0];
    auto v0_v2 = verts[2] - verts[0];
    auto dir = r.direction();
    auto parallel_vec = cross(dir, v0_v2);
    auto det = dot(v0_v1, parallel_vec);
    // If det < 0, this is a back-facing intersection, change hit_record front_face
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < EPS) return false;
    auto inv_det = 1. / det;


    // UV coordinates
    // https://www.cadnav.com/3d-models/model-54992.html
    double uu = 0, vv = 0;
    get_triangle_uv(rec.hit_point, uu, vv, verts, vert_uvs);

    auto tvec = r.origin() - verts[0];
    auto u = dot(tvec, parallel_vec) * inv_det;
    if (u < 0 || u > 1) return false;

    auto qvec = cross(tvec, v0_v1);
    auto v = dot(dir, qvec) * inv_det;
    if (v < 0 || u + v > 1) return false;

    double t = dot(v0_v2, qvec) * inv_det;

    if (t < ray_t.min || t > ray_t.max) return false;

    rec.t = t;
    rec.u = uu;
    rec.v = vv;
    rec.hit_point = r.at(t);
    rec.mat = mat_ptr;

    rec.front_face = true;
    vector3 normal = middle_normal;
    if (smooth_normals) {
        double a = u, b = v, c = (1 - u - v);
        // What does u and v map to?
        normal = a * vert_normals[1] + b * vert_normals[2] + c * vert_normals[0];
    }

    rec.set_face_normal(r, (det >= -EPS) ? normal : -normal);
    return true;

}

aabb triangle::bounding_box() const
{
    return bbox;
}

double triangle::pdf_value(const point3& o, const vector3& v) const
{
    hit_record rec;
    if (!this->hit(ray(o, v), interval(EPS, infinity), rec, 0))
        return 0;

    // from https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=4121581
    vector3 R1 = verts[0] - o, R2 = verts[1] - o, R3 = verts[2] - o;
    constexpr double r1 = R1.length();
    constexpr double r2 = R2.length();
    constexpr double r3 = R3.length();
    double N = dot(R1, cross(R2, R3));
    double D = r1 * r2 * r3 + dot(R1, R2) * r3 + dot(R1, R3) * r2 + dot(R2, R3) * r3;

    double omega = atan2(N, D);

    return 1. / omega;
}

vector3 triangle::random(const point3& o) const
{
    // From https://math.stackexchange.com/questions/18686/uniform-random-point-in-triangle-in-3d
    double r1 = random_double(), r2 = random_double();
    double ca = (1. - sqrt(r1)), cb = sqrt(r1) * (1. - r2), cc = r2 * sqrt(r1);
    vector3 random_in_triangle = verts[0] * ca + verts[1] * cb + verts[2] * cc;
    return random_in_triangle - o;
}

/// <summary>
/// Update the internal AABB of the mesh.
/// Warning: run this when the mesh is updated.
/// </summary>
void triangle::updateBoundingBox()
{
    // to implement
}