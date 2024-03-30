#pragma once

#include "../constants.h"
#include <cmath>

class interval
{
public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {} // Default interval is empty
    interval(double _min, double _max) : min(_min), max(_max) {}
    interval(const interval& a, const interval& b) : min(std::fmin(a.min, b.min)), max(std::fmax(a.max, b.max)) {}


    // is value inside the interval ?
    bool contains(double x) const;

    // is value strictly inside the interval ?
    bool surrounds(double x) const;

    // clamp smaller or bigger value to the min/max interval values
    double clamp(double x) const;

    double size() const;

    interval expand(double delta) const;

    static const interval empty, universe;
};

interval operator+(const interval& ival, double displacement);
interval operator+(double displacement, const interval& ival);