//#ifndef BLEND_H
//#define BLEND_H
//
//#include "../constants.h"
//#include "../misc/ray.h"
//#include "../misc/color.h"
//#include "../textures/texture.h"
//#include "../primitives/hittable.h"
//#include "../materials/material.h"
//
//#include <algorithm>
//#include <iterator>
//#include <numeric>
//#include <stdexcept>
//#include <tuple>
//#include <vector>
//
//
// https://github.com/Rabbid76/ray-tracing-cpp/blob/main/ray-tracing-core/include/material/blend_materials.h
//class blend : public material
//{
//public:
//    using Materials = std::vector<std::pair<double, const material*>>;
//
//private:
//    Materials materials;
//
//public:
//
//    blend(const Materials& materials_and_weights)
//    {
//        double sum = std::accumulate(
//            materials_and_weights.begin(), materials_and_weights.end(), 0.0,
//            [](const double& a, const Materials::value_type& b) -> double
//            {
//                return a + b.first;
//            });
//        materials = materials_and_weights;
//        double current_sum = 0.0;
//        for (auto& material : materials)
//        {
//            current_sum += material.first;
//            material.first = current_sum / sum;
//        }
//    }
//
//    ~blend() override = default;
//
//
//    const material* material() const
//    {
//        auto random_weight = math::RandomGenerator().random_size();
//        for (auto& material : materials)
//        {
//            if (random_weight < material.first)
//                return material.second;
//        }
//        return materials.back().second;
//    }
//
//
//    inline bool hit(const hit_record& hit_record) const override;
//    inline bool scatter(const ray& ray, hit_record& hit_record, scatter_record& scatter_record) const override;
//    inline math::Distance scattering_pdf(const ray& ray_in, const hit_record& hit_recocrd, const ray& scattered) const override;
//    inline color emit(const ray& ray_in, const hit_record& hit_record) const override;
//    inline bool has_texture() const override;
//    inline bool has_mask() const override;
//};
//
//
//
//
//
//bool hit(const hit_record& hit_record) const
//{
//    throw std::runtime_error("internal error");
//}
//
//bool scatter(const ray, hit_record& hit_record, scatter_record& scatter_record) const
//{
//    throw std::runtime_error("internal error");
//}
//
//math::Distance scattering_pdf(const ray& ray_in, const hit_record& hit_recocrd, const ray& scattered) const
//{
//    throw std::runtime_error("internal error");
//}
//
//core::Color emit(const math::Ray& ray_in, const core::HitRecord& hit_record) const
//{
//    throw std::runtime_error("internal error");
//}
//
//bool has_texture() const
//{
//    throw std::runtime_error("internal error");
//}
//
//bool has_mask() const
//{
//    throw std::runtime_error("internal error");
//}
//
//
//#endif