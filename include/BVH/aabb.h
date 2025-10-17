#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "vec3.h"
#include "ray.hpp"

class aabb {
public:
    static const aabb empty, universe;

    aabb() {} // all intervals are empty by default

    aabb(const interval& x, const interval& y, const interval& z) : 
        X(x), Y(y), Z(z) {}
    
    // along the positive direction of each axis
    aabb(const point3& a, const point3& b);

    aabb(const aabb& bbox1, const aabb& bbox2);

    const interval& axis_interval(int n) const;

    bool hit(const ray& r, interval& ray_t) const;

    int longes_axis() const {
        if(X.size() > Y.size())
            return X.size() > Z.size() ? 0 : 2;
        else 
            return Y.size() > Z.size() ? 1 : 2;
    }

private:
    interval X, Y, Z;
};

#endif