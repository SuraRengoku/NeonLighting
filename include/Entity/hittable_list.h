#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "aabb.h"
#include "hittable.hpp"
#include "general.hpp"
#include <vector>
#include "interval.h"

using std::vector;

class hittable_list : public hittable {
public:
    vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object);

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    aabb bounding_box() const override { return bbox; }

private:
    aabb bbox; // initially empty
};

#endif