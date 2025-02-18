#include "perlin.h"

#include "../utilities/math_utils.h"

//#include "../randomizers/pcg_randomizer.h"

perlin::perlin()
{
    ranvec = new vector3[point_count];

    randomizer rnd(DefaultRNGSeed);

    //std::shared_ptr<randomizer> rnd = std::make_shared<pcg_randomizer>();

    //randomizer rnd(1, "MySeed");

    for (int i = 0; i < point_count; ++i)
    {
        ranvec[i] = unit_vector(rnd.get_vector3(-1, 1));
    }

    perm_x = perlin_generate_perm(rnd);
    perm_y = perlin_generate_perm(rnd);
    perm_z = perlin_generate_perm(rnd);
}

perlin::~perlin()
{
    delete[] ranvec;
    delete[] perm_x;
    delete[] perm_y;
    delete[] perm_z;
}

double perlin::noise(const point3& p) const
{
    auto u = p.x - floor(p.x);
    auto v = p.y - floor(p.y);
    auto w = p.z - floor(p.z);

    u = u * u * (3 - 2 * u);
    v = v * v * (3 - 2 * v);
    w = w * w * (3 - 2 * w);

    auto i = static_cast<int>(floor(p.x));
    auto j = static_cast<int>(floor(p.y));
    auto k = static_cast<int>(floor(p.z));
    vector3 c[2][2][2];

    for (int di = 0; di < 2; di++)
        for (int dj = 0; dj < 2; dj++)
            for (int dk = 0; dk < 2; dk++)
                c[di][dj][dk] = ranvec[
                    perm_x[(i + di) & 255] ^
                        perm_y[(j + dj) & 255] ^
                        perm_z[(k + dk) & 255]
                ];

    return perlin_interp(c, u, v, w);
}

double perlin::turb(const point3& p, int depth) const
{
    auto accum = 0.0;
    auto temp_p = p;
    auto weight = 1.0;

    for (int i = 0; i < depth; i++) {
        accum += weight * noise(temp_p);
        weight *= 0.5;
        temp_p *= 2;
    }

    return fabs(accum);
}

int* perlin::perlin_generate_perm(randomizer& rnd)
{
    auto p = new int[point_count];

    for (int i = 0; i < perlin::point_count; i++)
        p[i] = i;

    permute(p, point_count, rnd);

    return p;
}

void perlin::permute(int* p, int n, randomizer& rnd)
{
    for (int i = n - 1; i > 0; i--) {
        int target = rnd.get_int(0, i);
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
}

double perlin::trilinear_interp(double c[2][2][2], double u, double v, double w)
{
    auto accum = 0.0;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++)
                accum += (i * u + (1 - i) * (1 - u)) *
                (j * v + (1 - j) * (1 - v)) *
                (k * w + (1 - k) * (1 - w)) * c[i][j][k];

    return accum;
}

double perlin::perlin_interp(vector3 c[2][2][2], double u, double v, double w)
{
    auto uu = u * u * (3 - 2 * u);
    auto vv = v * v * (3 - 2 * v);
    auto ww = w * w * (3 - 2 * w);
    auto accum = 0.0;

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++) {
                vector3 weight_v(u - i, v - j, w - k);
                accum += (i * uu + (1 - i) * (1 - uu))
                    * (j * vv + (1 - j) * (1 - vv))
                    * (k * ww + (1 - k) * (1 - ww))
                    * dot(c[i][j][k], weight_v);
            }

    return accum;
}