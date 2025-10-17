#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray {
public:
    ray() {}

    ray(const point3& origin, const vec3& direction, double time) : orig(origin), dir(direction), tm(time) {}

    ray(const point3& origin, const vec3& direction) : ray(origin, direction, 0) {} // delegate constructor

    const point3& origin() const { return orig; }
    const vec3& direction() const { return dir; }

    double time() const { return tm; }

    point3 at(double t) const {
        return orig + t * dir;
    }
private:
    // using point3 = vec3;
    point3 orig;
    vec3 dir;
    // for spaceTime
    double tm;
};

#endif