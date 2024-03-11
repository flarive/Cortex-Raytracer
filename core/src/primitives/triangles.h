#ifndef INCLUDE_TRIANGLES_H
#define INCLUDE_TRIANGLES_H

#include "../misc/ray.h"
#include "../misc/vec2.h"
#include "../misc/vec3.h"
#include "hittable.h"
#include "../aabb.h"

//#include "../../libs/hcon/include/FlatAlg.hpp"

#include <vector>

class TriangleHitable;

class TriangleBVH
{
  TriangleBVH *l = nullptr, *r = nullptr;
  aabb box;
  
  int num_inds = 0;
  int* inds = nullptr;

  friend class TriangleHitable;
};



class TriangleHitable : public hittable
{
  std::vector<vec3> vertices;
  std::vector<vec2> uvs;
  std::vector<vec3> normals;
  std::vector<int> indices;

  std::vector<float> diffuse_texture;

  int num_triangles;
  
  shared_ptr<material> mat;
  TriangleBVH * bvh_root;
  
public:
  TriangleHitable();
  TriangleHitable(const std::string& file_name, shared_ptr<material> _material);

  ~TriangleHitable();

  void print_bvh(TriangleBVH* tri, int depth);
  
  TriangleBVH* construct_bvh_tree(const std::vector<int>& inds);
  static void deconstruct_bvh_tree(TriangleBVH* node);

  void fill_triangle_min_coord(std::vector<std::pair<float, int>>& min_coords,
			       const std::vector<int>& inds,
			       int coord);

  void compute_separating_boxes(const std::vector<std::pair<float, int>>& vals,
				aabb& b0,
				aabb& b1);

  aabb compute_aabb(const std::vector<int>& inds) const;
  float separation_score(const aabb& b0, const aabb& b1);
  
  
  
  bool hit_triangle_bvh(TriangleBVH* node, const ray& r, interval ray_t, hit_record& rec, int depth) const;
  bool hit_triangle(const ray& r, interval ray_t, hit_record& rec, int ind) const;
  virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const;
  virtual bool bounding_box(float t0, float t1, aabb& box) const;
};

#endif
