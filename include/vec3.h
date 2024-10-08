#ifndef VEC3_H
#define VEC3_H

#include "general.h"

class vec3 {
public:
    double e[3];
    vec3() : e{0, 0, 0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator-=(const vec3& v) {
        e[0] -= v.e[0];
        e[1] -= v.e[1];
        e[2] -= v.e[2];
        return *this;
    }

    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1/t;
    }

    double length() const {
        return std::sqrt(length_squared());
    }

    double length_squared() const {
//         int res = 0;
// #pragma unroll 3
//         for(int i = 0; i < 3; ++i) 
//             res += e[i] * e[i];
//         return res;
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    /*
     * brief if the randomly generated vector is exactly opposite to the normal, 
     * the two will sum to zero, which will result in a zero scatter direction vector.
     * This leads to bad scenarios later on (infinities and NaNs)
     */
    bool near_zero() const {
        //return true if the vector is close to zero in all dimensions
        auto s = 1e-8;
        //std::fabs() returns the absolute value of its input
        return (std::fabs(e[0] < s) && (std::fabs(e[1]) < s) && (std::fabs(e[2])) < s);
    }

    static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};

//point3 is just a alias of vec3
using point3 = vec3;

//inline utilities, unable to expand, but save the calling consumption
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
    return u.e[0] * v.e[0] 
         + u.e[1] * v.e[1] 
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}
//normalization;
inline vec3 unit_vector(const vec3& v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    while(true) {
        auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
        if(p.length_squared() < 1)
            return p;
    }
}

//rejection model
inline vec3 random_unit_vector() {
    while(true) {
        auto p = vec3::random(-1, 1);
        auto lensq = p.length_squared();
        //vector outside the unit sphere or extremly small will be discarded
        if(1e-160 < lensq && lensq <= 1)
            return p / sqrt(lensq);
    }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = random_unit_vector();
    //make sure the generated random vector is in the right hemisphere
    return dot(on_unit_sphere, normal) > 0.0 ? on_unit_sphere : -on_unit_sphere;
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

/*
 * @param uv incident ray
 * @param n normal
 * @param etao_over_etai refractive index of outside material over counterpart of inside material
 */
inline vec3 refract(const vec3& uv, const vec3& n, double etao_over_etai) {
    //cos_theta of two vectors: cos_theta = a · b / |a|*|b|, uv and n are all unit vectors
    auto cos_theta = std::fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etao_over_etai * (uv + cos_theta * n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}
#endif