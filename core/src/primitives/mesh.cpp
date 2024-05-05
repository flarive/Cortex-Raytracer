//#include "mesh.h"
//
//#include "../utilities/rtw_stb_obj_loader.h"
//
//mesh::mesh()
//{
//}
//
//mesh::mesh(std::string filepath, std::shared_ptr<material> mat, bool use_mtl, bool use_smoothing)
//{
//	auto model = rtw_stb_obj_loader::load_model_from_file(filepath, mat, use_mtl, use_smoothing);
//}
//
//
//bool mesh::hit(const ray& r, interval ray_t, hit_record& rec, int depth) const
//{
//	vector3 normal(0, 0, 0);
//
//	float thit = 10000000.0f;
//	bool hit = false;
//
//	for (Triangle f : faces)
//	{
//		if (f.hit(r, ray_t.min, ray_t.max, rec))
//		{
//			if (rec.t < thit)
//			{
//				thit = rec.t;
//				normal = rec.normal;
//			}
//			hit = true;
//		}
//
//	}
//
//	rec.t = thit;
//	//if (Vec3<float>::dot(normal, r.direction()) > 0) normal = -normal;	
//	rec.normal = normal;
//	rec.hit_point = r.point_at_parameter(rec.t);
//
//	return hit;
//}
//
//aabb mesh::bounding_box() const
//{
//	return m_bbox;
//}
