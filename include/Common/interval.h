#ifndef INTERVAL_H
#define INTERVAL_H

#include "general.hpp"

class interval {
  public:
    double min, max;
    interval() : min(+infinity), max(-infinity) {}  // default is empty
    interval(double min, double max) : min(min), max(max) {}

    interval(const interval& ia, const interval& ib);

    double size() const {
        return max - min;
    }
    bool contains(double x) const {
        return min <= x && x <= max;
    }
    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const;

    interval expand(double delta) const;

    static const interval empty, universe;
};


#endif