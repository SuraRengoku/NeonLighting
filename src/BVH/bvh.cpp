#include "bvh.h"

bvh_node::bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
    // build the bounding box of the span of source objects.
    bbox = aabb::empty;
    for (size_t object_index = start; object_index < end; ++object_index)
        bbox = aabb(bbox, objects[object_index]->bounding_box());

    int axis = bbox.longes_axis();

    auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;

    size_t object_span = end - start;  // how many objects are there in the list

    if (object_span == 1) {
        left = right = objects[start];
    } else if (object_span == 2) {
        left  = objects[start];
        right = objects[start + 1];
    } else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + object_span / 2;
        left     = std::make_shared<bvh_node>(objects, start, mid);
        right    = std::make_shared<bvh_node>(objects, mid, end);
    }

    // bbox = aabb(left -> bounding_box(), right -> bounding_box());
}

bool bvh_node::hit(const ray& r, interval ray_t, hit_record& rec) const {
    if (!bbox.hit(r, ray_t))
        return false;

    bool hit_left = left->hit(r, ray_t, rec);
    /*
     *  we want a closer bbox. Thus, if the left bbox is hit, we only do search between [ray_t.min,
     * rec.t]. If the right bbox is closer, it will return a right value, otherwise we still hit the
     * left bbox. However, when the left box is not hit, we do search on the whole ray interval.
     */
    bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

    return hit_left || hit_right;
}

bool bvh_node::box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b,
                           int axis_index) {
    auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
    auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
    return a_axis_interval.min < b_axis_interval.min;
}