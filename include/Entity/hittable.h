#ifndef HITTABLE_H
#define HITTABLE_H

#include <memory>

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
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

    virtual aabb bounding_box() const = 0;
};

class translate : public hittable {
  public:
    translate(shared_ptr<hittable> object, const vec3& offset);
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    aabb bounding_box() const override {
        return bbox;
    }

  private:
    shared_ptr<hittable> object;
    vec3 offset;

    aabb bbox;
};

enum rotateAxis { X, Y, Z };

class rotate : public hittable {
  public:
    rotate(shared_ptr<hittable> object, rotateAxis axis, double angle);

    vec3 rotWtoO(const double& x, const double& y, const double& z) const;

    vec3 rotOtoW(const double& x, const double& y, const double& z) const;

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    aabb bounding_box() const override {
        return bbox;
    }

  private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
    rotateAxis axis;
};

#endif