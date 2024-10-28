#pragma once

#include "types.h"
#include <string>
#include <unordered_map>
#include <tuple>

class TriangleCache
{
public:
    static std::tuple<double, vector3> get_area_and_normal(size_t shape_index, size_t triangle_index, const vector3& v0, const vector3& v1, const vector3& v2);

private:
    static std::unordered_map<std::string, std::tuple<double, vector3>> cache;
};