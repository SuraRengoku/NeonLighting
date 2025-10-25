#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <random>

#include "color.hpp"
#include "general.hpp"
#include "perlin.h"
#include "rtw_stb_image.h"

class texture {
  public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
  public:
    solid_color(const color& _albedo) : albedo(_albedo) {}

    solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

    color value(double u, double v, const point3& p) const override {
        return albedo;
    }

  private:
    color albedo;
};

class checker_texture : public texture {
  public:
    checker_texture(double scale, shared_ptr<texture> even, shared_ptr<texture> odd)
        : inv_scale(1.0 / scale), even(even), odd(odd) {}

    checker_texture(double scale, const color& c1, const color& c2)
        : checker_texture(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

    color value(double u, double v, const point3& p) const override;

  private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

class image_texture : public texture {
  public:
    image_texture(const char* filename) : image(filename) {}

    color value(double u, double v, const point3& p) const override;

  private:
    rtw_image image;
};

class noise_texture : public texture {
  public:
    noise_texture(double _scale, int turb, int _freq = 4, std::mt19937 rng = get_default_rng())
        : scale(_scale), turbulence(turb), noise(_freq, rng) {}

    color value(double u, double v, const point3& p) const override {
        return color(0.5, 0.5, 0.5) *
               (1 + std::sin(scale * p.z() + 10 * noise.turb(p, turbulence)));
    }

  private:
    perlin noise;
    double scale;
    int turbulence;
};

#endif