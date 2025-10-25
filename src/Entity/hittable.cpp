#include <cmath>
#include <memory>

#include "general.hpp"
#include "hittable.h"
#include "texture.h"
#include "vec3.h"

translate::translate(std::shared_ptr<hittable> object, const vec3& offset)
    : object(object), offset(offset) {
    bbox = object->bounding_box() + offset;
}

bool translate::hit(const ray& r, interval ray_t, hit_record& rec) const {
    // move the ray backwards by the offset
    ray offset_r(r.origin() - offset, r.direction(), r.time());

    // determine whether an intersection exists along the offset ray (and if so, where)
    if (!object->hit(offset_r, ray_t, rec))
        return false;

    // move the intersection point forwards by the offset
    rec.p += offset;
    return true;
}

rotate::rotate(shared_ptr<hittable> object, rotateAxis axis, double angle)
    : object(object), axis(axis) {
    auto radians = degrees_to_radians(angle);
    sin_theta    = std::sin(radians);
    cos_theta    = std::cos(radians);
    bbox         = object->bounding_box();  // might be wrong

    point3 min(infinity, infinity, infinity);
    point3 max(-infinity, -infinity, -infinity);

    // do iteration on 8 vertices
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 2; ++k) {
                auto x = i * bbox.getX().max + (1 - i) * bbox.getX().min;
                auto y = j * bbox.getY().max + (1 - j) * bbox.getY().min;
                auto z = k * bbox.getZ().max + (1 - k) * bbox.getZ().min;

                vec3 tester = rotOtoW(x, y, z);

                for (int c = 0; c < 3; ++c) {
                    min[c] = std::fmin(min[c], tester[c]);
                    max[c] = std::fmax(max[c], tester[c]);
                }
            }
        }
    }

    bbox = aabb(min, max);
}

vec3 rotate::rotWtoO(const double& x, const double& y, const double& z) const {
    switch (axis) {
        case rotateAxis::X:
            return vec3(x, cos_theta * y + sin_theta * z, cos_theta * z - sin_theta * y);
        case rotateAxis::Y:
            return vec3(cos_theta * x - sin_theta * z, y, sin_theta * x + cos_theta * z);
        case rotateAxis::Z:
            return vec3(cos_theta * x + sin_theta * y, cos_theta * y - sin_theta * x, z);
    }
    return vec3(x, y, z);
}

vec3 rotate::rotOtoW(const double& x, const double& y, const double& z) const {
    switch (axis) {
        case rotateAxis::X:
            return vec3(x, cos_theta * y - sin_theta * z, sin_theta * y + cos_theta * z);
        case rotateAxis::Y:
            return vec3(cos_theta * x + sin_theta * z, y, cos_theta * z - sin_theta * x);
        case rotateAxis::Z:
            return vec3(cos_theta * x - sin_theta * y, sin_theta * x + cos_theta * y, z);
    }
    return vec3(x, y, z);
}

bool rotate::hit(const ray& r, interval ray_t, hit_record& rec) const {
    // transform the ray from world space to object space
    auto origin    = static_cast<point3>(rotWtoO(r.origin().x(), r.origin().y(), r.origin().z()));
    auto direction = rotWtoO(r.direction().x(), r.direction().y(), r.direction().z());

    ray rotated_r(origin, direction, r.time());

    // determine whether an intersection exists in object space (and if so, where)

    if (!object->hit(rotated_r, ray_t, rec))
        return false;

    // transform the intersection from object space back to world space
    rec.p      = static_cast<point3>(rotOtoW(rec.p.x(), rec.p.y(), rec.p.z()));
    rec.normal = rotOtoW(rec.normal.x(), rec.normal.y(), rec.normal.z());

    return true;
}