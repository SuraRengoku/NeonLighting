#include "texture.h"

color checker_texture::value(double u, double v, const point3& p) const {
    auto xInteger = int(floor(inv_scale * p.x()));
    auto yInteger = int(floor(inv_scale * p.y()));
    auto zInteger = int(floor(inv_scale * p.z()));

    bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;
    return isEven ? even->value(u, v, p) : odd->value(u, v, p);
}

color image_texture::value(double u, double v, const point3& p) const {
    if (image.height() <= 0)  // no image data will show cyan color
        return color(0, 1, 1);

    /*
    UV coordinates (in rendering):        image coordinates (in storing):
            v=1  ┌─────┐                       y=0  ┌─────┐
                 │     │                            │     │
                 │     │                            │     │
            v=0  └─────┘                       y=h  └─────┘
                 u=0   u=1                          x=0   x=w
    */
    // clamp input texture coordinates to [0, 1] x [1, 0]
    u = interval(0, 1).clamp(u);
    v = 1.0 - interval(0, 1).clamp(v);  // flip v to image coordinates

    auto i     = int(u * image.width());
    auto j     = int(v * image.height());
    auto pixel = image.pixel_data(i, j);

    auto color_scale = 1.0 / 255.0;
    return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
}
