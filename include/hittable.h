#ifndef HITTABLE_H
#define HITTABLE_H

#include "general.h"

class hit_record {
    public:
    point3 p;
    vec3 normal;
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