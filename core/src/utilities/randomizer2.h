#pragma once

#include "types.h"
#include <string>
#include <random>
#include "Util.h"

#include "../constants.h"
#include "pcg/pcg_random.hpp"

// https://github.com/define-private-public/PSRayTracing/blob/master/render_library/Util.hpp#L70


const std::string DefaultRNGSeed = "ASDF";

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
class _GeneralizedRandomGenerator {
private:
    const std::string _RandomStringChars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Data
    Distributor<Type> _rng_distribution;
    std::seed_seq _seed;
    RNG _rng;


    //std::uniform_real_distribution<double> uniformZeroOne; // ???????????

public:
    explicit _GeneralizedRandomGenerator(const std::string &rng_seed) : // noexcept :
        _rng_distribution(0, 1),
        _seed(rng_seed.begin(), rng_seed.end()),
        _rng(_seed)
    { }

    inline Type get_real() { // noexcept {
        return _rng_distribution(_rng);
    }

    inline Type get_real(const Type min, const Type max) { // noexcept {
        return min + ((max - min) * get_real());
    }

    inline int get_int(const int min, const int max) { // noexcept {
        return static_cast<int>(get_real(
            static_cast<Type>(min),
            static_cast<Type>(max + 1)
        ));
    }

    inline vector3 get_vec3() { // noexcept {
        return vector3(get_real(), get_real(), get_real());
    }

    inline vector3 get_vec3(const Type lower, const Type upper) { // noexcept {
        return vector3(
            get_real(lower, upper),
            get_real(lower, upper),
            get_real(lower, upper)
        );
    }

    // ??????????????????????????????????????? same as get_real ???????
    inline double getZeroOne()
    {
        //return uniformZeroOne(rng);

        double d = _rng_distribution(_rng);
        return d;
    }

    inline vector3 get_in_unit_sphere() { // noexcept {
        // BOOK CODE: (loop, with brancing, super bad... but it's acutely faster)
        while (true) {
            const vector3 p = get_vec3(-1, 1);
            if (p.length_squared() >= 1)
                continue;

            return p;
        }

//        // This is adapted from https://karthikkaranth.me/blog/generating-random-points-in-a-sphere/
//        //   It was an attempt to generate the points in a more performant maner, but turned out to
//        //   be slightly slower, and a little different too.
//        const rreal r = std::cbrt(get_real(0, 1));
//        const rreal theta = get_real(0, TwoPi);
//        const rreal phi = /* std::acos(get_real(0, 2) - 1); */ HalfPi - util::asin(get_real(0, 2) - 1);       // Use our asin approximation with the acos trig identity
//
//        const rreal sin_theta = util::sin(theta);
//        const rreal cos_theta = util::cos(theta);
//        const rreal sin_phi = util::sin(phi);
//        const rreal cos_phi = util::cos(phi);
//
//        const rreal x = r * sin_phi * cos_theta;
//        const rreal y = r * sin_phi * sin_theta;
//        const rreal z = r * cos_phi;
//
//        return Vec3(x, y, z);
    }

    inline vector3 get_in_unit_hemisphere(const vector3 &normal) { // noexcept {
        const vector3 in_unit_sphere = get_in_unit_sphere();
        return (in_unit_sphere.dot(normal) > 0) ? in_unit_sphere : -in_unit_sphere;
    }

    inline vector3 get_in_unit_disk() { // noexcept {
        // BOOK CODE: (loop, with branching, super bad... but it's acutely faster)
        while (true) {
            const vector3 p(get_real(-1, 1), get_real(-1, 1), 0);
            if (p.length_squared() >= 1)
                continue;

            return p;
        }

//        // This is polar, it's slightly slower (by 0.8%) for some odd reason...
//        const rreal r = util::sqrt(get_real(0, 1));
//        const rreal theta = get_real(0, TwoPi);
//
//        const rreal x = r * util::cos(theta);
//        const rreal y = r * util::sin(theta);
//
//        return Vec3(x, y, 0);
    }

    inline vector3 get_unit_vector() { // noexcept {
        const Type a = get_real(0, M_2_PI);
        const Type z = get_real(-1, 1);
        const double r = util::sqrt(1 - (z * z));

        return vector3(r * util::cos(a), r * util::sin(a), z);
    }

    vector3 get_cosine_direction() { // noexcept {
        const Type r1 = get_real();
        const Type r2 = get_real();
        const double phi = 2 * M_PI * r1;
        const double z = util::sqrt(1 - r2);
        const double r2_sqrt = util::sqrt(r2);
        const double x = util::cos(phi) * r2_sqrt;
        const double y = util::sin(phi) * r2_sqrt;

        return vector3(x, y, z);
    }

    vector3 get_to_sphere(const Type radius, const Type distance_squared) {
        const Type r1 = get_real();
        const Type r2 = get_real();
        const double phi = 2 * M_PI * r1;

        const double z = 1 + r2 * (util::sqrt(1 - (radius * radius / distance_squared)) - 1);
        const double zeta = util::sqrt(1 - (z * z));

        const double x = util::cos(phi) * zeta;
        const double y = util::sin(phi) * zeta;

        return vector3(x, y, z);
    }


    // Returns a random string (of the requested size)
    // Will be populated with [0-9a-zA-Z]
    std::string get_random_string(const size_t num) { //noexcept {
        std::string str(num, '0');
        const int max = static_cast<int>(_RandomStringChars.size()) - 1;

        // Pick out the characters
        for (auto &s : str)
            s = _RandomStringChars[static_cast<size_t>(get_int(0, max))];

        return str;
    }
};


// The Book uses the built-in meresenne twister engine to generate random numbers, it's a little slower
//#define RNG_ENGINE std::mt19937

// faster and should avoid black speckles
#define RNG_ENGINE pcg32



/*!
 * The random generator that should actually be used by the app in general places.
 */
class randomizer2 final : public _GeneralizedRandomGenerator<std::uniform_real_distribution, double, RNG_ENGINE> {
public:
    explicit randomizer2(const std::string &rng_seed) :
        _GeneralizedRandomGenerator(rng_seed)
    { }
};
