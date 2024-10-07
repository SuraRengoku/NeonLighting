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

    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 refelcted = reflect(ray_in.direction(), rec.normal);
        scattered = ray(rec.p, refelcted);
        attenuation = this -> albedo;
        return true;
    }

    private:
    color albedo;
};

class fuzzy : public material {
    public:
    fuzzy(const color& albedo_, double fuzz = 0.5) : albedo(albedo_), fuzz(fuzz < 1 ? fuzz : 1) {}
    
    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        vec3 reflected = reflect(ray_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + fuzz * random_unit_vector();
        scattered = ray(rec.p, reflected);
        attenuation = this -> albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

    private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
    public:
    dielectric(double refraction_index_) : refraction_index(refraction_index_) {}


    /*
     * @param attenuation always 1 since the glass(dielectric) surface absorbs nothing
     */
    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        //from outside to inside / from inside to outside
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(ray_in.direction());

        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        // when a ray enters a medium of lower index of refraction at a 
        // sufficiently glancing angle, it can refract with an angle greater than 90%, which
        // is impossible, we call it full reflected 
        bool cannot_refract = ri * sin_theta > 1.0;
        
        vec3 direction;
        if(cannot_refract || reflectance(cos_theta, ri) > random_double()) 
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);
        return true;
    }

    private:
    // refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index; // inside / outside

    static double reflectance(double cosine, double refraction_index) {
        //Schlick's approximation
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

#endif