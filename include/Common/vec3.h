#ifndef VEC3_H
#define VEC3_H

#include <random>

#include "general.hpp"

class vec3 {
  public:
    double e[3];
    vec3() : e{0, 0, 0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const {
        return e[0];
    }
    double y() const {
        return e[1];
    }
    double z() const {
        return e[2];
    }

    vec3 operator-() const {
        return vec3(-e[0], -e[1], -e[2]);
    }
    double operator[](int i) const {
        return e[i];
    }
    double& operator[](int i) {
        return e[i];
    }

    vec3& operator+=(const vec3& v);

    vec3& operator-=(const vec3& v);

    vec3& operator*=(double t);

    vec3& operator/=(double t);

    double length() const;

    double length_squared() const;

    /**
     * @brief if the randomly generated vector is exactly opposite to the normal,
     * the two will sum to zero, which will result in a zero scatter direction vector.
     * This leads to bad scenarios later on (infinities and NaNs)
     */
    bool near_zero() const;

    static vec3 random();

    static vec3 random(double min, double max);
};

// point3 is just a alias of vec3
using point3 = vec3;

// inline utilities, unable to expand, but save the calling consumption
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1], u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}
// normalization;
inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk(std::mt19937& rng) {
    while (true) {
        auto p = vec3(random_double_t_safe(-1, 1, rng), random_double_t_safe(-1, 1, rng), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double_t_safe(-1, 1), random_double_t_safe(-1, 1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

// rejection model
inline vec3 random_unit_vector(std::mt19937& rng) {
    while (true) {
        auto p     = vec3::random(-1, 1);
        auto lensq = p.length_squared();
        // vector outside the unit sphere or extremly small will be discarded
        if (1e-160 < lensq && lensq <= 1)
            return p / sqrt(lensq);
    }
}

inline vec3 random_on_hemisphere(const vec3& normal, std::mt19937& rng) {
    vec3 on_unit_sphere = random_unit_vector(rng);
    // make sure the generated random vector is in the right hemisphere
    return dot(on_unit_sphere, normal) > 0.0 ? on_unit_sphere : -on_unit_sphere;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

/**
 * @param uv incident ray
 * @param n normal
 * @param etao_over_etai refractive index of outside material over counterpart of inside material
 */
inline vec3 refract(const vec3& uv, const vec3& n, double etao_over_etai) {
    // cos_theta of two vectors: cos_theta = a · b / |a|*|b|, uv and n are all unit vectors
    auto cos_theta      = std::fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp     = etao_over_etai * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}
#endif