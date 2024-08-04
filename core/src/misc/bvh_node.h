#pragma once

#include "ray.h"
#include "hit_record.h"
#include "../primitives/hittable.h"
#include "../primitives/hittable_list.h"
#include "../utilities/interval.h"
#include "aabb.h"


//#include <omp.h>

//#include <future>
#include <algorithm>
//#include <vector>
//#include <memory>


/// <summary>
/// Bounding volume hierarchy
/// </summary>
class bvh_node : public hittable
{
public:


    // Helper function to partition the array
//    template<typename T, typename Comparator>
//    int partition(std::vector<T>& arr, int low, int high, Comparator comp) {
//        T pivot = arr[high];
//        int i = low - 1;
//
//        for (int j = low; j < high; ++j) {
//            if (comp(arr[j], pivot)) {
//                ++i;
//                std::swap(arr[i], arr[j]);
//            }
//        }
//        std::swap(arr[i + 1], arr[high]);
//        return i + 1;
//    }
//
//    // Parallel quicksort using OpenMP
//    template<typename T, typename Comparator>
//    void parallel_quicksort(std::vector<T>& arr, int low, int high, Comparator comp, int depth = 0) {
//        if (low < high) {
//            int pivot_index = partition(arr, low, high, comp);
//
//            // Parallel sections for left and right parts of the pivot
//#pragma omp parallel sections if (depth < 4) // limit depth to avoid too many threads
//            {
//#pragma omp section
//                {
//                    parallel_quicksort(arr, low, pivot_index - 1, comp, depth + 1);
//                }
//#pragma omp section
//                {
//                    parallel_quicksort(arr, pivot_index + 1, high, comp, depth + 1);
//                }
//            }
//        }
//    }

    bvh_node(const hittable_list& list, std::string name = "");
    bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects, size_t start, size_t end, std::string name = "");

    bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;
    aabb bounding_box() const override;


    



private:
    std::shared_ptr<hittable> m_left;
    std::shared_ptr<hittable> m_right;
    aabb m_bbox;

    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;

    static bool box_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis_index);
    static bool box_x_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b);
    static bool box_y_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b);
    static bool box_z_compare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b);
};