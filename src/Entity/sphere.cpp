#include "sphere.h"

sphere::sphere(const point3& sCenter, const point3& eCenter, double radius,
               shared_ptr<material> mat)
    : center(sCenter, eCenter - sCenter), radius(std::fmax(0, radius)), mat(mat) {
    // for dynamic sphere, the bounding box cover all intervals it moves
    auto rvec = vec3(radius, radius, radius);
    aabb boxS(center.at(0) - rvec, center.at(0) + rvec);
    aabb boxE(center.at(1) - rvec, center.at(1) + rvec);
    bbox = aabb(boxS, boxE);
}

sphere::sphere(const point3& staticCenter, double radius, shared_ptr<material> mat)
    : center(staticCenter, vec3(0, 0, 0)), radius(std::fmax(0, radius)), mat(mat) {
    // generate bounding box
    auto rvec = vec3(radius, radius, radius);
    bbox      = aabb(staticCenter - rvec, staticCenter + rvec);
}

bool sphere::hit(const ray& r, interval ray_t, hit_record& rec) const {
    point3 cur_center = center.at(r.time());
    vec3 oc           = cur_center - r.origin();
    auto a            = r.direction().length_squared();
    auto h            = dot(r.direction(), oc);
    auto c            = oc.length_squared() - radius * radius;

    auto discriminant = h * h - a * c;
    if (discriminant < 0)
        return false;
    auto sqrtd = std::sqrt(discriminant);

    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!ray_t.surrounds(root))
            return false;
    }
    rec.t = root;
    rec.p = r.at(rec.t);
    // rec.normal = (rec.p - center) / radius;
    vec3 outward_normal =
        (rec.p - cur_center) / radius;  // unit vector from sphere center to intersection point
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat = this->mat;
    return true;
}

void sphere::get_sphere_uv(const point3& p, double& u, double& v) {
    auto theta = std::acos(-p.y());
    auto phi   = std::atan2(-p.z(), p.x()) + pi;

    u = phi / (2 * pi);
    v = theta / pi;
}