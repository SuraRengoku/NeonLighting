#include <memory>

#include "bvh.h"
#include "camera.h"
#include "general.hpp"
#include "hittable.hpp"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "texture.h"

void staticScene();
void bouncing_spheres();
void checkered_spheres();
void earth();

int main() {
    earth();
}

void staticScene() {
    hittable_list world;

    auto R = std::cos(pi / 4);

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8));
    auto material_right  = make_shared<fuzzy>(color(0.8, 0.6, 0.2), 0.5);
    auto material_small  = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);

    world.add(make_shared<sphere>(point3(0.2, 0.0, -1.2), 0.4, material_center));
    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.1, material_ground));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.4, material_right));
    world.add(make_shared<sphere>(point3(-0.35, -0.2, -1.0), 0.2, material_small));
    world.add(make_shared<sphere>(point3(-0.35, -0.2, -1.0), 0.15, material_bubble));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;  // by default
    cam.image_width       = 800;         // by default
    cam.samples_per_pixel = 30;
    cam.max_depth         = 20;

    cam.vfov     = 20;
    cam.lookfrom = point3(-2, 2, 1);
    cam.lookat   = point3(0, 0, -1);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 10.0;
    cam.focus_dist    = 3.4;

    cam.render(world);

    // progress reporting
    std::clog << "\rDone.                    \n";
}

void bouncing_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    auto ground_material = make_shared<lambertian>(checker);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; ++a) {
        for (int b = -11; b < 11; ++b) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.65) {
                    // diffuse
                    auto albedo     = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto eCenter    = center + vec3(0, random_double_t_safe(0, 0.5), 0);
                    world.add(make_shared<sphere>(center, eCenter, 0.2, sphere_material));
                } else if (choose_mat < 0.8) {
                    // fuzzy metal
                    auto albedo     = color::random(0.5, 1);
                    auto fuzz       = random_double_t_safe(0, 0.5);
                    sphere_material = make_shared<fuzzy>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo     = color::random(0.5, 1);
                    sphere_material = make_shared<metal>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    sphere_material = make_shared<dielectric>(1.50);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.50);
    world.add(make_shared<sphere>(point3(0, 1, 1), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5));
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render_acc(world);

    std::clog << "\rDone.                    \n";
}

void checkered_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    camera cam;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render_acc(world);

    std::clog << "\rDone.                    \n";
}

void earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe         = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat   = point3(0, 0, 0);
    cam.vup      = vec3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.render_acc(hittable_list(globe));

    std::clog << "\rDone.                    \n";
}