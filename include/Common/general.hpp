#ifndef GENERAL_H
#define GENERAL_H

#include <cmath>
#include <cstdlib>
#include <random>
#include <limits>
#include <memory>
// #include "color.h"
// #include "ray.h"
// #include "vec3.h"
// #include "interval.h"

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline std::mt19937& get_default_rng() {
    static std::mt19937 rng{std::random_device{}()};
    return rng;
}

inline double random_double() {
    return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double_t_safe(std::mt19937& rng) {
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(rng);
}

//alternative
inline double random_double_G() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double_t_safe(double min, double max, std::mt19937& rng = get_default_rng()) {
    return min + (max - min) * random_double_t_safe(rng);
}


inline int random_int_t_safe(int min, int max, std::mt19937& rng) {
    return int(random_double_t_safe(min, max + 1, rng));
}


 
#endif