#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <vector>

#include "aabb.h"
#include "general.hpp"
#include "hittable.h"
#include "interval.h"

using std::vector;

class hittable_list : public hittable {
  public:
    vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) {
        add(object);
    }

    void clear() {
        objects.clear();
    }

    void add(shared_ptr<hittable> object);

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override;

    aabb bounding_box() const override {
        return bbox;
    }

  private:
    aabb bbox;  // initially empty
};

#endif