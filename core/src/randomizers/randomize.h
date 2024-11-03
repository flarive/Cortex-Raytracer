#pragma once

#include "../utilities/types.h"
#include "../utilities/Util.h"

#include "../constants.h"


#include <string>
#include <random>


class randomize
{
public:
    randomize() = default;
    virtual ~randomize() = default;



    virtual double get_real() noexcept = 0;


    virtual double get_real(const double min, const double max) noexcept = 0;


    virtual int get_int(const int min, const int max) noexcept = 0;


    virtual vector3 get_vector3() noexcept = 0;


    virtual vector3 get_vector3(const double lower, const double upper) noexcept = 0;

    virtual vector3 get_in_unit_sphere() noexcept = 0;


    virtual vector3 get_in_unit_hemisphere(const vector3& normal) noexcept = 0;


    virtual vector3 get_in_unit_disk() noexcept = 0;


    virtual vector3 random_to_sphere(double radius, double distance_squared) noexcept = 0;


    ///// <summary>
    ///// Check if a random unit vector is in the correct hemisphere (Figure 13)
    ///// </summary>
    ///// <param name="normal"></param>
    ///// <returns></returns>
    virtual vector3 random_on_hemisphere(const vector3& normal) noexcept = 0;


    virtual vector3 get_unit_vector() noexcept = 0;


    virtual vector3 get_cosine_direction() noexcept = 0;


    virtual vector3 get_to_sphere(const double radius, const double distance_squared) noexcept = 0;



    // Returns a random string (of the requested size)
    // Will be populated with [0-9a-zA-Z]
    virtual std::string get_random_string(const size_t num) noexcept = 0;


protected:
    const std::string m_randomStringChars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Data
    //Distributor<double> m_rng_distribution;
    std::seed_seq m_seed;
    //RNG m_rng;
};