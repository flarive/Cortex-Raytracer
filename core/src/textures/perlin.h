#pragma once

#include "../utilities/types.h"
#include "../utilities/randomizer2.h"

class perlin
{
public:
    perlin();
    ~perlin();

    double noise(const point3& p) const;
    double turb(const point3& p, int depth = 7) const;

private:
    static const int point_count = 256;
    vector3* ranvec;
    int* perm_x;
    int* perm_y;
    int* perm_z;

    static int* perlin_generate_perm(randomizer2& rnd);
    static void permute(int* p, int n, randomizer2& rnd);
    static double trilinear_interp(double c[2][2][2], double u, double v, double w);
    static double perlin_interp(vector3 c[2][2][2], double u, double v, double w);
};