#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "ray.hpp"
#include "vec3.h"

class aabb {
  public:
    static const aabb empty, universe;

    aabb() {}  // all intervals are empty by default

    aabb(const interval& x, const interval& y, const interval& z) : X(x), Y(y), Z(z) {
        pad_to_minimums();
    }

    // along the positive direction of each axis
    aabb(const point3& a, const point3& b);

    aabb(const aabb& bbox1, const aabb& bbox2);

    const interval& axis_interval(int n) const;

    bool hit(const ray& r, interval& ray_t) const;

    int longes_axis() const;

    friend aabb operator+(const aabb& bbox, const vec3& offset);

    friend aabb operator+(const vec3& offset, const aabb& bbox);

    interval getX() {
        return X;
    }
    interval getY() {
        return Y;
    }
    interval getZ() {
        return Z;
    }

  private:
    interval X, Y, Z;

    void pad_to_minimums();
};

#endif