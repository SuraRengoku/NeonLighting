#include <random>

#include "camera.h"
#include "general.hpp"

void camera::initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height >= 1) ? image_height : 1;

    sqrt_spp = static_cast<int>(std::sqrt(samples_per_pixel));
    // pixel_samples_scale = 1.0 / samples_per_pixel;
    pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);
    recip_sqrt_spp      = 1.0 / sqrt_spp;

    center = lookfrom;

    // auto focal_length = (lookfrom - lookat).length();
    auto theta = degrees_to_radians(vfov);
    auto h     = std::tan(theta / 2);
    // auto viewport_height = 2.0 * h * focal_length;
    auto viewport_height = 2.0 * h * focus_dist;
    auto viewport_width  = viewport_height * (double(image_width) / image_height);

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    // horizontal and vertical vectors
    auto viewport_u = viewport_width * u;
    auto viewport_v = viewport_height * (-v);

    // step length
    pixel_delta_u = viewport_u / image_width;   // width of each pixel
    pixel_delta_v = viewport_v / image_height;  // height of each pixel

    // define initial point
    auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    // define starting pixel location, e.g the middle point of the most upper-left pixel
    pixel100_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // calculate the camera defocus disk basis vectors
    auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u      = u * defocus_radius;
    defocus_disk_v      = v * defocus_radius;
}

void camera::render(const hittable& world) {
    initialize();
    // std::random_device rd;
    // std::mt19937 rng(rd()); // fixed for single thread
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = 0; j < image_height; ++j) {
        // std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
        std::clog << "\rProgress: " << std::setprecision(2) << std::fixed
                  << ((double)j / image_height) * 100 << "%" << std::flush;
        // ProgressBar(((double)j / image_height));
        for (int i = 0; i < image_width; ++i) {
            // auto pixel_center = pixel100_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            // auto ray_direction = pixel_center - center;
            // ray r(center, ray_direction);
            // color pixel_color = ray_color(r, world);
            // write_color(std::cout, pixel_color);
            color pixel_color(0, 0, 0);
            /*For a single pixel composed of multiple samples,
             we'll select samples from the area surrounding the pixel and
             average the resulting light (color) values together.
             */
            for (int s_j = 0; s_j < sqrt_spp; ++s_j) {
                for (int s_i = 0; s_i < sqrt_spp; ++s_i) {
                    ray r = get_ray(i, j, s_i, s_j);
                    pixel_color += ray_color(r, max_depth, world);
                }
            }
            write_color(std::cout, pixel_samples_scale * pixel_color);
        }
    }
}

void camera::render_acc(const hittable& world) {
    initialize();
    int W = image_width, H = image_height;
    std::vector<color> framebuffer(W * H);

    int nthreads = std::thread::hardware_concurrency();
    if (nthreads == 0)
        nthreads = 4;

    auto worker = [&](int tid) {
        std::random_device rd;  // for each thread, we have a independent random seed
        std::mt19937 rng(rd() + tid);

        int h0 = (H * tid) / nthreads;
        int h1 = (H * (tid + 1)) / nthreads;
        for (int j = h0; j < h1; ++j) {
            for (int i = 0; i < W; ++i) {
                color pixel_color(0, 0, 0);
                for (int s_j = 0; s_j < sqrt_spp; ++s_j) {
                    for (int s_i = 0; s_i < sqrt_spp; ++s_i) {
                        ray r = get_ray(i, j, s_i, s_j);
                        pixel_color += ray_color(r, max_depth, world);
                    }
                }
                framebuffer[j * W + i] = pixel_samples_scale * pixel_color;
            }
        }
    };

    std::clog << "Rendering framebuffer..." << std::endl;

    std::vector<std::thread> threads;
    for (int t = 0; t < nthreads; ++t)
        threads.emplace_back(worker, t);
    for (auto& th : threads)
        th.join();

    std::clog << "Rendering Finished." << std::endl;
    std::clog << "Writing..." << std::endl;

    std::cout << "P3\n" << W << ' ' << H << "\n255\n";
    for (int j = 0; j < H; ++j) {
        for (int i = 0; i < W; ++i)
            write_color(std::cout, framebuffer[j * W + i]);
    }
}

void camera::ProgressBar(double progress) {
    const int barWidth = 50;
    int pos            = barWidth * progress;
    std::clog << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)
            std::clog << "=";
        else if (i == pos)
            std::clog << ">";
        else
            std::clog << " ";
    }
    std::clog << "]" << int(progress * 100.0) << "%\r";
    std::clog.flush();
}

ray camera::get_ray(int i, int j, int s_i, int s_j, std::mt19937& rng) const {
    auto offset = sample_square_stratified(s_i, s_j, rng);
    // auto offset = sample_square(rng);
    // current pixel location: pixel100_loc + i * pixel_delta_u + j * pixel_delta_v, e.g. the
    // middle point of current pixel
    auto pixel_sample =
        pixel100_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);
    auto ray_origin    = (defocus_angle <= 0) ? center : defocus_disk_sample(rng);
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time      = random_double_t_safe(rng);

    return ray(ray_origin, ray_direction, ray_time);
}

vec3 camera::sample_square(std::mt19937& rng = get_default_rng()) const {
    // random_double() range : [0, 1) -> random_double() - 0.5 range : [-0.5, 0.5)
    return vec3(random_double_t_safe(rng) - 0.5, random_double_t_safe(rng) - 0.5, 0);
}

/**
 * @brief an uniform distribution on [-0.5, 0.5) by using stratification, making edges sharper and
 * helping reduce aliasing and moire pattern
 * @param s_i the row index of gird
 * @param s_j the column index of grid
 */
vec3 camera::sample_square_stratified(int s_i, int s_j,
                                      std::mt19937& rng = get_default_rng()) const {
    auto px = ((s_i + random_double_t_safe(rng)) * recip_sqrt_spp) - 0.5;
    auto py = ((s_j + random_double_t_safe(rng)) * recip_sqrt_spp) - 0.5;
    return vec3(px, py, 0);
}

point3 camera::defocus_disk_sample(std::mt19937& rng = get_default_rng()) const {
    auto p = random_in_unit_disk(rng);  // only in x-y plane
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}

color camera::ray_color(const ray& r, int depth, const hittable& world, std::mt19937& rng) const {
    if (depth <= 0)
        return color(0, 0, 0);
    hit_record rec;
    // 0.001 to overlook hits that are very close to the calculated intersection point to avoid
    // self-reflection

    // if the ray hits nothing, return the background color
    if (!world.hit(r, interval(0.001, infinity), rec))
        return background;

    // vec3 direction = Lambertian_Ref ? rec.normal + random_unit_vector() :
    // random_on_hemisphere(rec.normal);
    // //return 70% of the color from a bounce
    // return 0.7 * ray_color(ray(rec.p, direction), depth - 1, world);//recursive

    ray scattered;
    color attenuation;
    color color_from_emission = rec.mat->emitting(rec.u, rec.v, rec.p);
    if (!rec.mat->scatter(r, rec, attenuation, scattered, rng))
        return color_from_emission;

    color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);

    return color_from_scatter + color_from_emission;
}