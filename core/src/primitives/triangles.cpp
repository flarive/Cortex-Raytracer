#include "triangles.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../../libs/obj/tinyobjloader.hpp"

#include "../aabb.h"
#include "../misc/vec3.h"
#include "../misc/vec2.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

// https://github.com/TheVaffel/weekend-raytracer


void triangleHitable::print_bvh(triangleBVH* bvh, int depth = 0)
{
  if(bvh->num_inds)
  {
    std::cout << std::setw(depth) << "\tnum_indices = " << bvh->num_inds << ", first = " << bvh->inds[0] << ", last = " << bvh->inds[bvh->num_inds - 1] << std::endl;
  }
  else
  {
    std::cout << std::setw(depth) << "\tNon-leaf" << std::endl;
  }

  if(bvh->l) {
    print_bvh(bvh->l, depth + 1);
  }

  if(bvh->r) {
    print_bvh(bvh->r, depth + 1);
  }
}

triangleHitable::triangleHitable() { }

triangleHitable::triangleHitable(const std::string& file_name, shared_ptr<material> mat) : mat(mat)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_name.c_str());

  if(!warn.empty()) {
    std::cout << "[tinyobj warning] " << warn << std::endl;
  }

  if(!err.empty()) {
    std::cerr << "[tinyobj error] " << err << std::endl;
  }

  if (!ret) {
    exit(1);
  }

  for (size_t s = 0; s < shapes.size(); s++) {
    size_t index_offset = 0;
    for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      int fv = shapes[s].mesh.num_face_vertices[f];
      if(fv != 3) {
	std::cerr << "Only triangle faces allowed at this point" << std::endl;
	exit(1);
      }


      this->vertices.resize(attrib.vertices.size());
      this->normals.resize(attrib.vertices.size());
      this->uvs.resize(attrib.vertices.size());

      for(int i = 0; i < fv; i++)
      {
	tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + i];

	vec3 vertex(attrib.vertices[3 * idx.vertex_index + 0],
			  attrib.vertices[3 * idx.vertex_index + 1],
			  attrib.vertices[3 * idx.vertex_index + 2]);
	this->vertices[idx.vertex_index] = vertex;

	if(attrib.normals.size()) {
	  vec3 normal(attrib.normals[3 * idx.vertex_index + 0],
			  attrib.normals[3 * idx.vertex_index + 1],
			  attrib.normals[3 * idx.vertex_index + 2]);

	  this->normals[idx.vertex_index] = normal;
	}
	if(attrib.texcoords.size()) {
	  vec2 uv(attrib.texcoords[2 * idx.vertex_index + 0],
			attrib.texcoords[2 * idx.vertex_index + 1]);
	  this->uvs[idx.vertex_index] = uv;
	}


	indices.push_back(idx.vertex_index);
      }

      index_offset += fv;
    }
  }

  this->num_triangles = indices.size() / 3;

  std::cout << "Num triangles = " << this->num_triangles << std::endl;

  std::vector<int> indices_range(this->num_triangles);
  for(int i = 0; i < this->num_triangles; i++) {
    indices_range[i] = i;
  }



  std::cout << "[TriangleHitable()] Inds: " << indices_range.size() << " elems, first = " << indices_range[0] << ", last: " << (*(indices_range.end() - 1)) << std::endl;

  std::cout << "Constructing BVH tree for " << file_name << std::endl;
  this->bvh_root = this->construct_bvh_tree(indices_range);
  std::cout << "Finished constructing BVH" << std::endl;

  // print_bvh(this->bvh_root);
}

triangleHitable::~triangleHitable()
{
  triangleHitable::deconstruct_bvh_tree(this->bvh_root);
}

void triangleHitable::deconstruct_bvh_tree(triangleBVH* node)
{
  if (node->num_inds)
  {
    delete[] node->inds;
  }

  if (node->l) {
    triangleHitable::deconstruct_bvh_tree(node->l);
  }

  if (node->r) {
    triangleHitable::deconstruct_bvh_tree(node->r);
  }

  delete node;
}

// Compute minimum coord for each triangle
void triangleHitable::fill_triangle_min_coord(std::vector<std::pair<float, int>>& min_coords,
					      const std::vector<int>& inds,
					      int coord)
{
	for (unsigned int i = 0; i < inds.size(); i++)
	{
		double minx = -1e18;
		minx = std::min(minx, this->vertices[this->indices[3 * inds[i] + 0]][coord]);
		minx = std::min(minx, this->vertices[this->indices[3 * inds[i] + 1]][coord]);
		minx = std::min(minx, this->vertices[this->indices[3 * inds[i] + 2]][coord]);
		min_coords[i] = std::make_pair(minx, inds[i]);
	}
}

triangleBVH* triangleHitable::construct_bvh_tree(const std::vector<int>& inds) {


  std::cout << "[construct] Inds: " << inds.size() << " elems, first = " << inds[0] << ", last: " << (*(inds.end() - 1)) << std::endl;

  triangleBVH* bvh = new triangleBVH;

  bvh->box = this->compute_aabb(inds);
  std::cout << "Computed bounding box bvh " << bvh->box.min() << ", " << bvh->box.max() << std::endl;

  if(inds.size() <= 2)
  {
    bvh->inds = new int[inds.size()];
    bvh->num_inds = inds.size();
    std::cout << "Num inds in computing bvh: " << inds.size() << std::endl;

    memcpy(bvh->inds, inds.data(), bvh->num_inds * sizeof(int));
    return bvh;
  }

  std::vector<std::pair<float, int> > x_scores(inds.size());
  std::vector<std::pair<float, int> > y_scores(inds.size());
  std::vector<std::pair<float, int> > z_scores(inds.size());

  this->fill_triangle_min_coord(x_scores, inds, 0);
  this->fill_triangle_min_coord(y_scores, inds, 1);
  this->fill_triangle_min_coord(z_scores, inds, 2);

  // Sort triangles along the three axes
  std::sort(x_scores.begin(), x_scores.end());
  std::sort(y_scores.begin(), y_scores.end());
  std::sort(z_scores.begin(), z_scores.end());

  aabb bx0, bx1, by0, by1, bz0, bz1;

  // Compute axis-aligned bounding boxes dividing each sorted array in two
  this->compute_separating_boxes(x_scores, bx0, bx1);
  this->compute_separating_boxes(y_scores, by0, by1);
  this->compute_separating_boxes(z_scores, bz0, bz1);

  // Determine how good the separation is
  float x_score = separation_score(bx0, bx1);
  float y_score = separation_score(by0, by1);
  float z_score = separation_score(bz0, bz1);

  float max_score = std::max(x_score, std::max(y_score, z_score));


  std::vector<std::pair<float, int> >* vecref;

  if(max_score == x_score) {
    vecref = &x_scores;
    std::cout << "Dividing by x-axis" << std::endl;
  } else if (max_score == y_score) {
    vecref = &y_scores;
    std::cout << "Dividing by y-axis" << std::endl;
  } else if (max_score == z_score) {
    vecref = &z_scores;
    std::cout << "Dividing by z-axis" << std::endl;
  } else {
    std::cerr << "WHaT yOu NeveR pLAy TuBEr SiMULaToR? Also, numeric precision works differently, I guess" << std::endl;
    exit(-1);
  }


  unsigned int halfway = inds.size() / 2;

  std::vector<int> vec0(halfway);
  std::vector<int> vec1(inds.size() - halfway);

  for(unsigned int i = 0; i < inds.size(); i++) {
    if(i < halfway) {
      vec0[i] = (*vecref)[i].second;
    } else {
      vec1[i - halfway] = (*vecref)[i].second;
    }
  }

  bvh->l = construct_bvh_tree(vec0);
  bvh->r = construct_bvh_tree(vec1);

  return bvh;
}

void triangleHitable::compute_separating_boxes(const std::vector<std::pair<float, int>>& vals,
					       aabb& b0,
					       aabb& b1) {
  unsigned int half_way = vals.size() / 2;
  std::vector<int> vec0(half_way);
  std::vector<int> vec1(vals.size() - half_way);

  for(unsigned int i = 0; i < vals.size(); i++) {
    if(i < half_way) {
      vec0[i] = vals[i].second;
    } else {
      vec1[i - half_way] = vals[i].second;
    }
  }

  b0 = this->compute_aabb(vec0);
  b1 = this->compute_aabb(vec1);
}

aabb triangleHitable::compute_aabb(const std::vector<int>& inds) const
{
  aabb box;
  for(unsigned int i = 0; i < inds.size(); i++) {
    for(int j = 0; j < 3; j++) {
      int ind = this->indices[3 * inds[i] + j];
      box.add(this->vertices[ind]);
    }
  }

  return box;
}

float triangleHitable::separation_score(const aabb& b0, const aabb& b1)
{
  vec3 diff0 = b0.min() - b1.max();
  float max0 = std::max(diff0[0], std::max(diff0[1], diff0[2]));
  
  vec3 diff1 = b1.min() - b0.max();
  float max1 = std::max(diff1[0], std::max(diff1[1], diff1[2]));

  return std::max(max0, max1);
}

bool triangleHitable::hit_triangle(const ray& r, interval ray_t, hit_record& rec, int ind) const
{
  // The Moller-Trumbore algorithm
  vec3 v0 = this->vertices[this->indices[3 * ind + 0]];
  vec3 v1 = this->vertices[this->indices[3 * ind + 1]];
  vec3 v2 = this->vertices[this->indices[3 * ind + 2]];

  vec3 e1 = v1 - v0;
  vec3 e2 = v2 - v0;

  vec3 tvec = r.origin() - v0;
  vec3 dvec = r.direction();

  vec3 p = cross(dvec, e2);
  vec3 q = cross(tvec, e1);

  float det = dot(p, e1);
  if(std::abs(det) <= 1e-7) {
    // Ray and triangle nearly parallel
    return false;
  }

  vec3 tuv = (1.0 / det) * vec3(dot(q, e2), dot(p, tvec), dot(q, dvec));

  float t = tuv[0];
  float u = tuv[1];
  float v = tuv[2];

  if(t <= ray_t.min || t >= ray_t.max || (u + v) > 1 || v < 0 || u < 0) {
    return false;
  }

  rec.t = t;
  rec.p = r.origin() + dvec * t;
  rec.normal = normalize((this->normals[this->indices[3 * ind + 0]] * (1 - u - v) +
    this->normals[this->indices[3 * ind + 1]] * u +
		this->normals[this->indices[3 * ind + 2]] * v));

  rec.mat = mat;
  rec.u = u;
  rec.v = v;
  return true;
}

bool triangleHitable::hit_triangle_bvh(triangleBVH* node, const ray& r, interval ray_t, hit_record& rec, int depth) const
{
  if (node->box.hit(r, ray_t))
  {
    bool bb = false;

    if(node->num_inds)
    {
      for(int i = 0; i < node->num_inds; i++)
      {
	    if(this->hit_triangle(r, ray_t, rec, node->inds[i]))
        {
            ray_t.max = rec.t;
	        bb = true;
	    }
      }
    }

    if(node->l || node->r) {
      hit_record rl, rr;

      // Ensure rl.t is set
      rl.t = ray_t.max;

      bool hl = node->l ? hit_triangle_bvh(node->l, r, ray_t, rl, depth + 1) : false;

      // Update tmax, so we may skip hr if we're lucky
      ray_t.max = rl.t;

      bool hr = node->r ? hit_triangle_bvh(node->r, r, ray_t, rr, depth + 1) : false;

      if(hl && hr) {
	if(rl.t < rr.t) {
	  rec = rl;
	} else {
	  rec = rr;
	}
	return true;
      } else if (hl) {
	rec = rl;
	return true;
      } else if (hr) {
	rec = rr;
	return true;
      } else {
	return bb;
      }

    } else {
      return bb;
    }
  }

  return false;
}

bool triangleHitable::hit(const ray& r, interval ray_t, hit_record& rec) const
{
  // Naive intersection implementation
  /* bool hit_anything = false;
  for(int i = 0; i < this->num_triangles; i++) {
    if(hit_triangle(r, tmin, tmax, rec, i)) {
      tmax = rec.t;
      hit_anything = true;
    }
  }
  return hit_anything; */

  // Optimized bvh
  return this->hit_triangle_bvh(bvh_root, r, ray_t, rec, 0);
}

bool triangleHitable::bounding_box(float t0, float t1, aabb& box) const
{
  std::vector<int> inds(this->num_triangles);
  for(int i = 0; i < this->num_triangles; i++) {
    inds[i] = i;
  }

  box = this->compute_aabb(inds);

  return true;
}
