#ifndef SPHERE_H
#define SPHERE_H

#include "general.hpp"
#include "hittable.h"
#include "material.h"

class sphere : public hittable {
  public:
    // dynamic sphere
    sphere(const point3& sCenter, const point3& eCenter, double radius, shared_ptr<material> mat);

    // stationay sphere
    sphere(const point3& staticCenter, double radius, shared_ptr<material> mat);

    aabb bounding_box() const override {
        return bbox;
    }

    // std::fmax() returns the maximum of the two floating-point arguments, similarly with
    // std::fmin()
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    static void get_sphere_uv(const point3& p, double& u, double& v);

  private:
    ray center;
    double radius;
    shared_ptr<material> mat;
    aabb bbox;
};

#endif