#include "constant_medium.h"
#include "general.hpp"
#include "hittable.h"
#include "interval.h"

bool constant_medium::hit(const ray& r, interval ray_t, hit_record& rec) const {
    hit_record rec1, rec2;

    if (!boundary->hit(r, interval::universe, rec1))  // incident point
        return false;
    if (!boundary->hit(r, interval(rec1.t + 0.0001, infinity), rec2))  // injection point
        return false;

    if (rec1.t < ray_t.min)
        rec1.t = ray_t.min;
    if (rec2.t > ray_t.max)
        rec2.t = ray_t.max;

    if (rec1.t >= rec2.t)
        return false;

    if (rec1.t < 0)
        rec1.t = 0;

    auto ray_length = r.direction().length();
    auto distance_inside_boundary =
        (rec2.t - rec1.t) * ray_length;  // transfer range of ray in medium
    // use log distribution to simulate ray scattering
    // Beer-Lambert Law, ref: https://en.wikipedia.org/wiki/Beer%E2%80%93Lambert_law
    auto hit_distance =
        neg_inv_density *
        std::log(random_double_t_safe(get_default_rng()));  // TODO: multi-thread supports

    if (hit_distance > distance_inside_boundary)  // scatter out of medium
        return false;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    rec.normal     = vec3(1, 0, 0);  // arbitrary
    rec.front_face = true;           // arbitrary
    rec.mat        = phase_function;

    return true;
}