#ifndef QUAD_H
#define QUAD_H

#include <memory>

#include "hittable.h"
#include "hittable_list.h"
#include "material.h"

class quad : public hittable {
  public:
    quad(const point3& Q, const vec3& u, const vec3& v, std::shared_ptr<material> mat);

    virtual void set_bounding_box();

    aabb bounding_box() const override {
        return bbox;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    virtual bool is_interior(double a, double b, hit_record& rec) const;

  protected:
    point3 Q;  // starting corner
    vec3 u, v;
    vec3 w;
    std::shared_ptr<material> mat;
    aabb bbox;
    vec3 normal;
    double D;
};

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat) {
    // returns the 3D box (six sides) that contains the two opposite vertices a & b
    auto sides = std::make_shared<hittable_list>();

    // construct the two opposite vertices with the minimum and maximum coordinates
    auto min = point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
    auto max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0, 0);
    auto dy = vec3(0, max.y() - min.y(), 0);
    auto dz = vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat));   // front
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat));  // right
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat));  // back
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat));   // left
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat));  // top
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat));   // bottom

    return sides;
}

class tri : public quad {
  public:
    tri(const point3& o, const vec3& aa, const vec3& ab, shared_ptr<material>& mat)
        : quad(o, aa, ab, mat) {}

    virtual bool is_interior(double a, double b, hit_record& rec) const override;
};

class ellipse : public quad {
  public:
    ellipse(const point3& center, const vec3& sideA, const vec3& sideB, shared_ptr<material>& mat)
        : quad(center, sideA, sideB, mat) {}

    virtual void set_bounding_box() override;

    virtual bool is_interior(double a, double b, hit_record& rec) const override;
};

class annulus : public ellipse {
  public:
    annulus(const point3& center, const vec3& sideA, const vec3& sideB, double _inner,
            shared_ptr<material>& mat)
        : inner(_inner), ellipse(center, sideA, sideB, mat) {}

    virtual bool is_interior(double a, double b, hit_record& rec) const override;

  private:
    double inner;
};
#endif