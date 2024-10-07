#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "general.h"

class sphere : public hittable {
public:
    sphere(const point3& center, double radius, shared_ptr<material> mat) : 
            center(center), 
            radius(std::fmax(0, radius)), 
            mat(mat) {
        //TODO: Initialize the material pointer 'mat'. e.g. this -> mat
    }
    //std::fmax() returns the maximum of the two floating-point arguments, similarly with std::fmin()
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        vec3 oc = center - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c;
        if(discriminant < 0)
            return false;
        auto sqrtd = std::sqrt(discriminant);

        auto root = (h - sqrtd) / a;
        if(!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if(!ray_t.surrounds(root))
                return false;
        }
        rec.t = root;
        rec.p = r.at(rec.t);
        // rec.normal = (rec.p - center) / radius;
        vec3 outward_normal = (rec.p - center) / radius; //unit vector from sphere center to intersection point
        rec.set_face_normal(r, outward_normal);
        rec.mat = this -> mat;
        return true;
    }
private:
    point3 center;
    double radius;
    shared_ptr<material> mat;
};

#endif