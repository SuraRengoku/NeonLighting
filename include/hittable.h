#ifndef HITTABLE_H
#define HITTABLE_H

#include "interval.h"
#include "vec3.h"
#include "ray.h"

class material;

class hit_record {
    public:
    point3 p; //hit point
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0; // obtuse angle -> < 0; acute angle -> > 0
        normal = front_face ? outward_normal : -outward_normal;
    }
};
//some infos should only be calculated if the object is hitted

class hittable {
    public:
    virtual ~hittable() = default;
    //hit only occurs when if Tmin < t < Tmax
    virtual bool hit(const ray& r, interval ray_t, hit_record& res) const = 0;
};

#endif