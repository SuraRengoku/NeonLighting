#include "vec3.h"

vec3& vec3::operator+=(const vec3& v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
}

vec3& vec3::operator-=(const vec3& v) {
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
}

vec3& vec3::operator*=(double t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

vec3& vec3::operator/=(double t) {
    return *this *= 1 / t;
}

double vec3::length() const {
    return std::sqrt(length_squared());
}

double vec3::length_squared() const {
    // int res = 0;
    // #pragma unroll 3
    // for(int i = 0; i < 3; ++i)
    //     res += e[i] * e[i];
    // return res;
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}

bool vec3::near_zero() const {
    // return true if the vector is close to zero in all dimensions
    auto s = 1e-8;
    // std::fabs() returns the absolute value of its input
    return (std::fabs(e[0] < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2])) < s);
}

vec3 vec3::random() {
    return vec3(random_double(), random_double(), random_double());
}

vec3 vec3::random(double min, double max) {
    return vec3(random_double_t_safe(min, max), random_double_t_safe(min, max),
                random_double_t_safe(min, max));
}

