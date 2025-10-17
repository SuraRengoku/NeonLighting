#ifndef MATERIAL_H
#define MATERIAL_H

#include <random>

#include "color.hpp"
#include "general.hpp"
#include "hittable.hpp"
#include "texture.h"

class material {
  public:
    virtual ~material() = default;

    /**
     * @brief if scattered, say how much the ray should be attenuated
     * @param scattered produce a scattered ray (or say it absorbed the incident ray)
     */
    virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation,
                         ray& scattered, std::mt19937& rng = get_default_rng()) const {
        return false;
    }
};

class lambertian : public material {
  public:
    lambertian(const color& albedo_) : albedo(albedo_), tex(make_shared<solid_color>(albedo)) {}
    lambertian(shared_ptr<texture> _tex) : tex(_tex) {}

    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered,
                 std::mt19937& rng = get_default_rng()) const override;

  private:
    // keep the albedo for later use, or you can just use albedo in <solid_color>tex
    color albedo;  // fractional reflectance
    shared_ptr<texture> tex;
};

class metal : public material {
  public:
    metal(const color& albedo_) : albedo(albedo_) {}

    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered,
                 std::mt19937& rng = get_default_rng()) const override;

  private:
    color albedo;
};

class fuzzy : public material {
  public:
    fuzzy(const color& albedo_, double fuzz = 0.5) : albedo(albedo_), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered,
                 std::mt19937& rng = get_default_rng()) const override;

  private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
  public:
    dielectric(double refraction_index_) : refraction_index(refraction_index_) {}

    /**
     * @param attenuation always 1 since the glass(dielectric) surface absorbs nothing
     */
    bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered,
                 std::mt19937& rng = get_default_rng()) const override;

  private:
    // refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;  // inside / outside

    static double reflectance(double cosine, double refraction_index);
};

#endif