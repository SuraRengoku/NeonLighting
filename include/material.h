#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "color.h"

class material {
    public:
    virtual ~material() = default;

    /** 
     * @brief if scattered, say how much the ray should be attenuated
     * @param scattered produce a scattered ray (or say it absorbed the incident ray)
     */
    virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        return false;
    }
};

class lambertian : public material {
    public:
    lambertian(const color& albedo_) : albedo(albedo_) {}

    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        auto scatter_direction = rec.normal + random_unit_vector();
        
        //catch degenerate scatter direction
        if(scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = this -> albedo;
        return true;
    }

    private:
    color albedo; //fractional reflectance
};

class metal : public material {
    public:
    metal(const color& albedo_) : albedo(albedo_) {}

    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override{
        vec3 refelcted = reflect(ray_in.direction(), rec.normal);
        scattered = ray(rec.p, refelcted);
        attenuation = this -> albedo;
        return true;
    }

    private:
    color albedo;
};

#endif