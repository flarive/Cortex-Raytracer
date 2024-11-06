#pragma once

#include "../utilities/types.h"
#include <string>
#include <random>
#include "../utilities/util.h"

#include "../constants.h"
#include "pcg/pcg_random.hpp"

// https://github.com/define-private-public/PSRayTracing/blob/master/render_library/Util.hpp#L70


static constexpr std::string DefaultRNGSeed = "ASDF";

/*!
 * This is an RNG where you can plug in different types of distrubtions, generators
 * and types to generate.  It's not recommeded that you use this class directly,
 * unless you need to.  The reason that this exists is so that the randomly setup
 * scenes in the `Book*` can be setup the same, regardless if `rreal` is set to
 * `float` or `double` precision.
 *
 * Use the class `RandomGenerator` instead.
 */
template<template<class> class Distributor, class Type, class RNG>
class generalizedRandomGenerator
{
private:
    const std::string m_randomStringChars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Data
    Distributor<Type> m_rng_distribution;
    std::seed_seq m_seed;
    RNG m_rng;

public:
    explicit generalizedRandomGenerator(const std::string& rng_seed, int type) noexcept :
        m_rng_distribution(0, 1),
        m_seed(rng_seed.begin(), rng_seed.end()),
        m_rng(m_seed)
    { }

    inline Type get_real() noexcept
    {
        return m_rng_distribution(m_rng);
    }

    inline Type get_real(const Type min, const Type max) noexcept
    {
        return min + ((max - min) * get_real());
    }

    inline int get_int(const int min, const int max) noexcept
    {
        return static_cast<int>(get_real(
            static_cast<Type>(min),
            static_cast<Type>(max + 1)
        ));
    }

    inline vector3 get_vector3() noexcept
    {
        return vector3(get_real(), get_real(), get_real());
    }

    inline vector3 get_vector3(const Type lower, const Type upper) noexcept
    {
        return vector3(
            get_real(lower, upper),
            get_real(lower, upper),
            get_real(lower, upper)
        );
    }

    inline vector3 get_in_unit_sphere() noexcept
    {
        // BOOK CODE: (loop, with brancing, super bad... but it's acutely faster)
        while (true) {
            const vector3 p = get_vector3(-1, 1);
            if (vector_length_squared(p) >= 1)
                continue;

            return p;
        }
    }

    inline vector3 get_in_unit_hemisphere(const vector3& normal) noexcept
    {
        const vector3 in_unit_sphere = get_in_unit_sphere();
        return (in_unit_sphere.dot(normal) > 0) ? in_unit_sphere : -in_unit_sphere;
    }

    inline vector3 get_in_unit_disk() noexcept
    {
        // BOOK CODE: (loop, with branching, super bad... but it's acutely faster)
        while (true) {
            const vector3 p(get_real(-1, 1), get_real(-1, 1), 0);
            if (vector_length_squared(p) >= 1)
                continue;

            return p;
        }
    }


    inline vector3 random_to_sphere(double radius, double distance_squared) noexcept
    {
        const Type r1 = get_real(0.0, 1.0);
        const Type r2 = get_real(0.0, 1.0);
        Type z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

        Type phi = M_2_PI * r1;
        Type x = cos(phi) * sqrt(1 - z * z);
        Type y = sin(phi) * sqrt(1 - z * z);

        return vector3(x, y, z);
    }

    ///// <summary>
    ///// Check if a random unit vector is in the correct hemisphere (Figure 13)
    ///// </summary>
    ///// <param name="normal"></param>
    ///// <returns></returns>
    inline vector3 random_on_hemisphere(const vector3& normal) noexcept
    {
        vector3 on_unit_sphere = get_unit_vector();

        // In the same hemisphere as the normal
        if (glm::dot(on_unit_sphere, normal) > 0.0)
        {
            // good hemisphere facing normal
            return on_unit_sphere;
        }
        else
        {
            // wrong hemisphere not facing normal
            // invert the vector to make it be in the correct hemisphere
            return -on_unit_sphere;
        }
    }

    inline vector3 get_unit_vector() noexcept
    {
        const Type a = get_real(0, M_2_PI);
        const Type z = get_real(-1, 1);
        const double r = util::sqrt(1 - (z * z));

        return vector3(r * util::cos(a), r * util::sin(a), z);
    }

    vector3 get_cosine_direction() noexcept
    {
        const Type r1 = get_real();
        const Type r2 = get_real();
        const double phi = M_2_PI * r1;
        const double z = util::sqrt(1 - r2);
        const double r2_sqrt = util::sqrt(r2);
        const double x = util::cos(phi) * r2_sqrt;
        const double y = util::sin(phi) * r2_sqrt;

        return vector3(x, y, z);
    }

    vector3 get_to_sphere(const Type radius, const Type distance_squared) noexcept
    {
        const Type r1 = get_real();
        const Type r2 = get_real();
        const double phi = M_2_PI * r1;

        const double z = 1 + r2 * (util::sqrt(1 - (radius * radius / distance_squared)) - 1);
        const double zeta = util::sqrt(1 - (z * z));

        const double x = util::cos(phi) * zeta;
        const double y = util::sin(phi) * zeta;

        return vector3(x, y, z);
    }


    // Returns a random string (of the requested size)
    // Will be populated with [0-9a-zA-Z]
    std::string get_random_string(const size_t num) noexcept
    {
        std::string str(num, '0');
        const int max = static_cast<int>(m_randomStringChars.size()) - 1;

        // Pick out the characters
        for (auto& s : str)
            s = m_randomStringChars[static_cast<size_t>(get_int(0, max))];

        return str;
    }
};








// The Book uses the built-in meresenne twister engine to generate random numbers, it's a little slower
//#define RNG_ENGINE std::mt19937
//#define RNG_ENGINE std::minstd_rand

// faster and should avoid black speckles
#define RNG_ENGINE pcg32_fast


/*!
 * The random generator that should actually be used by the app in general places.
 */
class randomizer final : public generalizedRandomGenerator<std::uniform_real_distribution, double, RNG_ENGINE>
{
public:
    explicit randomizer(int type) : generalizedRandomGenerator(DefaultRNGSeed, type)
    {
    }

    explicit randomizer(const std::string& rng_seed, int type) : generalizedRandomGenerator(rng_seed, type)
    {
    }
};