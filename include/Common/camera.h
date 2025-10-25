#ifndef CAMERA_H
#define CAMERA_H

#include <iomanip>
#include <iostream>
#include <random>
#include <thread>

#include "color.hpp"
#include "general.hpp"
#include "hittable.h"
#include "material.h"
#include "vec3.h"

class camera {
  public:
    double aspect_ratio   = 16.0 / 9.0;
    int image_width       = 800;
    int samples_per_pixel = 20;
    int max_depth         = 10;  // maximum number of ray bounces into scene
    bool Lambertian_Ref   = true;
    color background;

    double vfov     = 90;  // vertical field of view
    point3 lookfrom = point3(0, 0, 0);
    point3 lookat   = point3(0, 0, -1);
    vec3 vup        = vec3(0, 1, 0);

    double defocus_angle = 0;
    double focus_dist    = 10;

    void render(const hittable& world);

    void render_acc(const hittable& world);

    void ProgressBar(double progress);

  private:
    int image_height;
    double pixel_samples_scale;  // color scale factor for a sum of pixel samples
    int sqrt_spp;                // square root of number of samples per pixel
    double recip_sqrt_spp;       // 1 / sqrt_spp
    point3 center;               // camera center
    point3 pixel100_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 u, v, w;  // right, up, opposite view direction
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;

    void initialize();

    ray get_ray(int i, int j, int s_i, int s_j, std::mt19937& rng = get_default_rng()) const;

    vec3 sample_square(std::mt19937& rng) const;

    vec3 sample_square_stratified(int s_i, int s_j, std::mt19937& rng) const;

    // for defocus blur
    point3 defocus_disk_sample(std::mt19937& rng) const;

    color ray_color(const ray& r, int depth, const hittable& world,
                    std::mt19937& rng = get_default_rng()) const;
};

#endif