#include "material.h"

bool lambertian::scatter(const ray& ray_in, const hit_record& rec, color& attenuation,
                         ray& scattered, std::mt19937& rng) const {
    auto scatter_direction = rec.normal + random_unit_vector(rng);

    // catch degenerate scatter direction
    if (scatter_direction.near_zero())
        scatter_direction = rec.normal;

    scattered   = ray(rec.p, scatter_direction, ray_in.time());  // scattered ray for next hit
    attenuation = this->tex->value(rec.u, rec.v, rec.p);
    return true;
}

bool metal::scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered,
                    std::mt19937& rng) const {
    vec3 refelcted = reflect(ray_in.direction(), rec.normal);
    scattered      = ray(rec.p, refelcted, ray_in.time());
    attenuation    = this->albedo;
    return true;
}

bool fuzzy::scatter(const ray& ray_in, const hit_record& rec, color& attenuation, ray& scattered,
                    std::mt19937& rng) const {
    vec3 reflected = reflect(ray_in.direction(), rec.normal);
    reflected      = unit_vector(reflected) + fuzz * random_unit_vector(rng);
    scattered      = ray(rec.p, reflected, ray_in.time());
    attenuation    = this->albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
}

bool dielectric::scatter(const ray& ray_in, const hit_record& rec, color& attenuation,
                         ray& scattered, std::mt19937& rng) const {
    attenuation = color(1.0, 1.0, 1.0);
    // from outside to inside / from inside to outside
    double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

    vec3 unit_direction = unit_vector(ray_in.direction());

    double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    // when a ray enters a medium of lower index of refraction at a
    // sufficiently glancing angle, it can refract with an angle greater than 90%, which
    // is impossible, we call it full reflected
    bool cannot_refract = ri * sin_theta > 1.0;

    vec3 direction;
    if (cannot_refract || reflectance(cos_theta, ri) > random_double_t_safe(rng))
        direction = reflect(unit_direction, rec.normal);
    else
        direction = refract(unit_direction, rec.normal, ri);

    scattered = ray(rec.p, direction, ray_in.time());
    return true;
}

double dielectric::reflectance(double cosine, double refraction_index) {
    // Schlick's approximation
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0      = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}