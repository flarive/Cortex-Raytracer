#include "mersenne_twister_randomizer.h"

#include "../utilities/util.h"



double mersenne_twister_randomizer::get_real() noexcept
{
    return m_rng_distribution(m_rng);
}

double mersenne_twister_randomizer::get_real(const double min, const double max) noexcept
{
    return min + ((max - min) * get_real());
}

int mersenne_twister_randomizer::get_int(const int min, const int max) noexcept
{
    return static_cast<int>(get_real(
        static_cast<double>(min),
        static_cast<double>(max + 1)
    ));
}

vector3 mersenne_twister_randomizer::get_vector3() noexcept
{
    return vector3(get_real(), get_real(), get_real());
}

vector3 mersenne_twister_randomizer::get_vector3(const double lower, const double upper) noexcept
{
    return vector3(
        get_real(lower, upper),
        get_real(lower, upper),
        get_real(lower, upper)
    );
}

vector3 mersenne_twister_randomizer::get_in_unit_sphere() noexcept
{
    // BOOK CODE: (loop, with brancing, super bad... but it's acutely faster)
    while (true) {
        const vector3 p = get_vector3(-1, 1);
        if (vector_length_squared(p) >= 1)
            continue;

        return p;
    }
}

vector3 mersenne_twister_randomizer::get_in_unit_hemisphere(const vector3& normal) noexcept
{
    const vector3 in_unit_sphere = get_in_unit_sphere();
    return (glm::dot(in_unit_sphere, normal) > 0) ? in_unit_sphere : -in_unit_sphere;

}

vector3 mersenne_twister_randomizer::get_in_unit_disk() noexcept
{
    // BOOK CODE: (loop, with branching, super bad... but it's acutely faster)
    while (true) {
        const vector3 p(get_real(-1, 1), get_real(-1, 1), 0);
        if (vector_length_squared(p) >= 1)
            continue;

        return p;
    }
}


vector3 mersenne_twister_randomizer::random_to_sphere(double radius, double distance_squared) noexcept
{
    const double r1 = get_real(0.0, 1.0);
    const double r2 = get_real(0.0, 1.0);
    double z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);

    double phi = 2 * M_PI * r1;
    double x = cos(phi) * sqrt(1 - z * z);
    double y = sin(phi) * sqrt(1 - z * z);

    return vector3(x, y, z);
}

///// <summary>
///// Check if a random unit vector is in the correct hemisphere (Figure 13)
///// </summary>
///// <param name="normal"></param>
///// <returns></returns>
vector3 mersenne_twister_randomizer::random_on_hemisphere(const vector3& normal) noexcept
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

vector3 mersenne_twister_randomizer::get_unit_vector() noexcept
{
    const double a = get_real(0, M_2_PI);
    const double z = get_real(-1, 1);
    const double r = util::sqrt(1 - (z * z));

    return vector3(r * util::cos(a), r * util::sin(a), z);
}

vector3 mersenne_twister_randomizer::get_cosine_direction() noexcept
{
    const double r1 = get_real();
    const double r2 = get_real();
    const double phi = 2 * M_PI * r1;
    const double z = util::sqrt(1 - r2);
    const double r2_sqrt = util::sqrt(r2);
    const double x = util::cos(phi) * r2_sqrt;
    const double y = util::sin(phi) * r2_sqrt;

    return vector3(x, y, z);
}

vector3 mersenne_twister_randomizer::get_to_sphere(const double radius, const double distance_squared) noexcept
{
    const double r1 = get_real();
    const double r2 = get_real();
    const double phi = 2 * M_PI * r1;

    const double z = 1 + r2 * (util::sqrt(1 - (radius * radius / distance_squared)) - 1);
    const double zeta = util::sqrt(1 - (z * z));

    const double x = util::cos(phi) * zeta;
    const double y = util::sin(phi) * zeta;

    return vector3(x, y, z);
}


// Returns a random string (of the requested size)
// Will be populated with [0-9a-zA-Z]
std::string mersenne_twister_randomizer::get_random_string(const size_t num) noexcept
{
    std::string str(num, '0');
    const int max = static_cast<int>(m_randomStringChars.size()) - 1;

    // Pick out the characters
    for (auto& s : str)
        s = m_randomStringChars[static_cast<size_t>(get_int(0, max))];

    return str;
}

