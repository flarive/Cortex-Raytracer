#pragma once

#include "randomizer.h"

#include "pcg_random.hpp"

/// <summary>
/// Fast and should avoid black speckles
/// </summary>
class pcg_randomizer : public randomize
{
public:
    double get_real() noexcept override;
    double get_real(const double min, const double max) noexcept override;
    int get_int(const int min, const int max) noexcept override;
    vector3 get_vector3() noexcept override;
    vector3 get_vector3(const double lower, const double upper) noexcept override;
    vector3 get_in_unit_sphere() noexcept override;
    vector3 get_in_unit_hemisphere(const vector3& normal) noexcept override;
    vector3 get_in_unit_disk() noexcept override;
    vector3 random_to_sphere(double radius, double distance_squared) noexcept override;
    vector3 random_on_hemisphere(const vector3& normal) noexcept override;
    vector3 get_unit_vector() noexcept override;
    vector3 get_cosine_direction() noexcept override;
    vector3 get_to_sphere(const double radius, const double distance_squared) noexcept override;
    std::string get_random_string(const size_t num) noexcept override;

private:
    std::uniform_real_distribution<double> m_rng_distribution;
    pcg32 m_rng;
};