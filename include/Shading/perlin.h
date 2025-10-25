#ifndef PERLIN_H
#define PERLIN_H

#include <random>

#include "general.hpp"
#include "vec3.h"

class perlin {
  public:
    // ref: https://zhuanlan.zhihu.com/p/620107368
    perlin(int _freq, std::mt19937 input_rng);

    double noise(const point3& p) const;

    double turb(const point3& p, int depth) const;

  private:
    static const int point_count = 256;
    static std::mt19937 rng;
    vec3 randvec[point_count];
    int perm_x[point_count];
    int perm_y[point_count];
    int perm_z[point_count];
    int frequency;

    static void perlin_generate_perm(int* p);

    static void permute(int* p, int n);

    static double trilinear_lerp(double c[2][2][2], double u, double v, double w);

    static double perlin_lerp(const vec3 c[2][2][2], double u, double v, double w);
};

#endif