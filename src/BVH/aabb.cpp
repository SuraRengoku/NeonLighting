#include "aabb.h"

const aabb aabb::empty = aabb(interval::empty, interval::empty, interval::empty);
const aabb aabb::universe = aabb(interval::universe, interval::universe, interval::universe);

aabb::aabb(const point3& a, const point3& b) {
    X = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
    Y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
    Z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
}

aabb::aabb(const aabb& bbox1, const aabb& bbox2) {
    X = interval(bbox1.X, bbox2.X);
    Y = interval(bbox1.Y, bbox2.Y);
    Z = interval(bbox1.Z, bbox2.Z);
}

const interval& aabb::axis_interval(int n) const {
    if(n == 1) return Y;
    if(n == 2) return Z;
    return X;
}


bool aabb::hit(const ray& r, interval& ray_t) const {
    const point3& ray_orig = r.origin();
    const vec3& ray_dir = r.direction(); 

    for(int axis = 0; axis < 3; ++axis) {
        const double dino = 1.0f / ray_dir[axis];
        const interval& axisI = axis_interval(axis); // align to each axis

        // supposed ray interval to cover the aabb 
        auto t0 = (axisI.min - ray_orig[axis]) * dino;
        auto t1 = (axisI.max - ray_orig[axis]) * dino; 

        if(t0 < t1) {
            if(t0 > ray_t.min) ray_t.min = t0;
            if(t1 < ray_t.max) ray_t.max = t1;
        } else {
            if(t1 > ray_t.min) ray_t.min = t1;
            if(t0 < ray_t.max) ray_t.max = t0;
        }

            // after processing all axes, if there is no ray interval left, the ray does not hit the aabb
        if(ray_t.max <= ray_t.min) 
            return false;
    }
    return true;
}