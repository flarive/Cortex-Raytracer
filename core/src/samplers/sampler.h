#pragma once

#include <atomic>

#include "../utilities/math_utils.h"
#include "../utilities/randomizer.h"

/// <summary>
/// https://github.com/VladChira/ray-tracer/tree/main/src/core/samplers
/// </summary>
class Sampler
{
public:
    Sampler();

    Sampler(const int num);

    Sampler(const int num, const int num_sets);

    virtual ~Sampler();

    Sampler(const Sampler &s);

    Sampler(Sampler &&s) noexcept;

    Sampler &operator=(const Sampler &s);

    Sampler &operator=(Sampler &&s) noexcept;

    virtual Sampler *clone() const = 0;

    void set_num_sets(const int np);

    virtual void // generate sample patterns in a unit square
    generate_samples() = 0;

    int get_num_samples();

    void shuffle_x_coordinates();

    void shuffle_y_coordinates();

    void setup_shuffled_indices();

    void map_samples_to_unit_disk();

    void map_samples_to_hemisphere(const float p);

    void map_samples_to_sphere();

    // the following functions are not const because they change count and jump

    Eigen::Vector2f // get next sample on unit square
    sample_unit_square();

    Eigen::Vector2f // get next sample on unit disk
    sample_unit_disk();

    Eigen::Vector3f // get next sample on unit hemisphere
    sample_hemisphere();

    Eigen::Vector3f // get next sample on unit sphere
    sample_sphere();

    Eigen::Vector2f           // only used to set up a vector noise table
    sample_one_set(); // this is not discussed in the book, but see the
                        // file LatticeNoise.cpp in Chapter 31

protected:
    int num_samples;                         // the number of sample points in a set
    int num_sets;                            // the number of sample sets
    std::vector<Eigen::Vector2f> samples;            // sample points on a unit square
    std::vector<int> shuffled_indices;       // shuffled samples array indices
    std::vector<Eigen::Vector2f> disk_samples;       // sample points on a unit disk
    std::vector<Eigen::Vector3f> hemisphere_samples; // sample points on a unit hemisphere
    std::vector<Eigen::Vector3f> sphere_samples;     // sample points on a unit sphere
    // unsigned long count;                     // the current number of sample points used
    // int jump;                                // random index jump
    std::atomic<unsigned long> count;
    std::atomic<int> jump;
};
