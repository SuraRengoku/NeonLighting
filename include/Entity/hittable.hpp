#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"
#include "interval.h"
#include "ray.hpp"
#include "vec3.h"

class material;

class hit_record {
  public:
    point3 p;  // hit point
    vec3 normal;
    shared_ptr<material> mat;
    double t;

    // for texture mapping
    double u;
    double v;

    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face =
            dot(r.direction(), outward_normal) < 0;  // obtuse angle -> < 0; acute angle -> > 0
        normal = front_face ? outward_normal : -outward_normal;
    }
};
// some infos should only be calculated if the object is hitted

class hittable {
  public:
    virtual ~hittable() = default;
    // hit only occurs when if Tmin < t < Tmax
    virtual bool hit(const ray& r, interval ray_t, hit_record& res) const = 0;

    virtual aabb bounding_box() const = 0;
};

#endif