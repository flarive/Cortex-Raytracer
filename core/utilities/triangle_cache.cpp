#include "triangle_cache.h"

#include <format>
#include <string>

std::tuple<double, vector3> TriangleCache::get_area_and_normal(size_t shape_index, size_t triangle_index, const vector3& v0, const vector3& v1, const vector3& v2)
{
    std::string key = std::format("{}_{}", shape_index, triangle_index);


    // Check if this triangle configuration is already cached
    auto it = cache.find(key);
    if (it != cache.end()) {
        return it->second;
    }

    // Calculate area and middle_normal if not cached
    double a = (v0 - v1).length();
    double b = (v1 - v2).length();
    double c = (v2 - v0).length();
    double s = (a + b + c) / 2.0;
    double area = glm::sqrt(fabs(s * (s - a) * (s - b) * (s - c)));
    vector3 middle_normal = unit_vector(glm::cross(v0 - v1, v0 - v2));

    // Cache the result
    cache[key] = { area, middle_normal };

    return { area, middle_normal };
}


// Define the static member here
std::unordered_map<std::string, std::tuple<double, vector3>> TriangleCache::cache;