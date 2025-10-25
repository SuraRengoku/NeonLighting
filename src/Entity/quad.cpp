#include <memory>

#include "hittable.h"
#include "hittable_list.h"
#include "quad.h"
#include "vec3.h"

quad::quad(const point3& Q, const vec3& u, const vec3& v, std::shared_ptr<material> mat)
    : Q(Q), u(u), v(v), mat(mat) {
    auto n = cross(u, v);
    normal = unit_vector(n);
    D      = dot(normal, Q);
    w      = n / dot(n, n);
    set_bounding_box();
}

void quad::set_bounding_box() {
    auto bbox_diagonal1 = aabb(Q, Q + u + v);
    auto bbox_diagnoal2 = aabb(Q + u, Q + v);
    bbox                = aabb(bbox_diagonal1, bbox_diagnoal2);  // union of diagonal bounding box
}

bool quad::hit(const ray& r, interval ray_t, hit_record& rec) const {
    auto denom = dot(normal, r.direction());

    if (std::fabs(denom) < 1e-8)  // parallel
        return false;

    auto t = (D - dot(normal, r.origin())) / denom;

    if (!ray_t.contains(t))  // beyond lighting range
        return false;

    auto intersection = r.at(t);

    vec3 planar_hitpt_vector = intersection - Q;
    auto alpha               = dot(w, cross(planar_hitpt_vector, v));
    auto beta                = dot(w, cross(u, planar_hitpt_vector));

    if (!is_interior(alpha, beta, rec))
        return false;

    rec.t   = t;
    rec.p   = intersection;
    rec.mat = mat;
    rec.set_face_normal(r, normal);

    return true;
}

bool quad::is_interior(double a, double b, hit_record& rec) const {
    interval unit_interval = interval(0, 1);

    if (!unit_interval.contains(a) || !unit_interval.contains(b))
        return false;

    rec.u = a;
    rec.v = b;
    return true;
}

bool tri::is_interior(double a, double b, hit_record& rec) const {
    if ((a < 0) || (b < 0) || (a + b > 1))
        return false;

    rec.u = a;
    rec.v = b;
    return true;
}

void ellipse::set_bounding_box() {
    // TODO:
    // bbox = aabb();
}

bool ellipse::is_interior(double a, double b, hit_record& rec) const {
    if ((a * a + b * b) > 1)  // [-1, 1]
        return false;

    rec.u = a / 2 + 0.5;  // [0, 1]
    rec.v = b / 2 + 0.5;
    return true;
}

bool annulus::is_interior(double a, double b, hit_record& rec) const {
    auto center_dist = sqrt(a * a + b * b);  // [-1, 1]
    if ((center_dist < inner) || (center_dist > 1))
        return false;

    rec.u = a / 2 + 0.5;  // [0, 1]
    rec.v = b / 2 + 0.5;
    return true;
}