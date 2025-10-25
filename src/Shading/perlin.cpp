#include <cmath>
#include <cstdint>
#include <random>

#include "general.hpp"
#include "math_utils.hpp"
#include "perlin.h"
#include "vec3.h"

std::mt19937 perlin::rng = get_default_rng();

perlin::perlin(int _freq, std::mt19937 input_rng) {
    for (int i = 0; i < point_count; ++i)
        randvec[i] = unit_vector(vec3::random(-1, 1));

    perlin_generate_perm(perm_x);
    perlin_generate_perm(perm_y);
    perlin_generate_perm(perm_z);

    frequency = _freq;
}

double perlin::noise(const point3& p) const {
    // auto i = int(frequency * p.x()) & 255;
    // auto j = int(frequency * p.y()) & 255;
    // auto k = int(frequency * p.z()) & 255;

    // return randfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];

    auto u = p.x() - std::floor(p.x());
    auto v = p.y() - std::floor(p.y());
    auto w = p.z() - std::floor(p.z());

    auto i = int(std::floor(p.x()));
    auto j = int(std::floor(p.y()));
    auto k = int(std::floor(p.z()));
    vec3 c[2][2][2];

    for (int di = 0; di < 2; ++di) {
        for (int dj = 0; dj < 2; ++dj) {
            for (int dk = 0; dk < 2; ++dk)
                c[di][dj][dk] = randvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^
                                        perm_z[(k + dk) & 255]];
        }
    }

    // return trilinear_lerp(c, u, v, w);
    return perlin_lerp(c, u, v, w);
}

double perlin::turb(const point3& p, int depth) const {
    auto accum  = 0.0;
    auto temp_p = p;
    auto weight = 1.0;

    for (int i = 0; i < depth; ++i) {
        accum += weight * noise(temp_p);
        weight *= 0.5;
        temp_p *= 2;
    }

    return std::fabs(accum);  // float absolute value
}

void perlin::permute(int* p, int n) {
    // Fisher-Yates shuffle
    // ref: https://zhuanlan.zhihu.com/p/565961597
    for (int i = n - 1; i > 0; --i) {
        int target = random_int_t_safe(0, i, rng);
        int tmp    = p[i];
        p[i]       = p[target];
        p[target]  = tmp;
    }
}

void perlin::perlin_generate_perm(int* p) {
    for (int i = 0; i < point_count; ++i)
        p[i] = i;  // [0, 1, ..., point_count - 1]

    permute(p, point_count);
}

double perlin::trilinear_lerp(double c[2][2][2], double u, double v, double w) {
    // use Hermitian Smoothing to avoid Mach bands
    auto uu = HermitianSmoothing(u);
    auto vv = HermitianSmoothing(v);
    auto ww = HermitianSmoothing(w);

    // ref: https://zhuanlan.zhihu.com/p/77496615
    auto accum = 0.0;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k)
                accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) *
                         (k * ww + (1 - k) * (1 - ww)) * c[i][j][k];
        }
    }
    return accum;
}

double perlin::perlin_lerp(const vec3 c[2][2][2], double u, double v, double w) {
    // use Hermitian Smoothing to avoid Mach bands
    auto uu = HermitianSmoothing(u);
    auto vv = HermitianSmoothing(v);
    auto ww = HermitianSmoothing(w);

    auto accum = 0.0;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                vec3 weight_v(u - i, v - j, w - k);
                accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) *
                         (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
            }
        }
    }

    // range [-1, 1], do not forget to remap to range[0, 1]
    return accum;
}