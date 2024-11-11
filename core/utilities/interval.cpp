#include "interval.h"

interval::interval() : min(+infinity), max(-infinity)
{
}

interval::interval(double _min, double _max) : min(_min), max(_max)
{
}

interval::interval(const interval& a, const interval& b) : min(std::fmin(a.min, b.min)), max(std::fmax(a.max, b.max))
{
}

bool interval::contains(double x) const
{
    // is value inside the interval ?
    return min <= x && x <= max;
}

bool interval::surrounds(double x) const
{
    // is value strictly inside the interval ?
    return min < x && x < max;
}

double interval::clamp(double x) const
{
    // clamp smaller or bigger value to the min/max interval values
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

double interval::size() const
{
    return max - min;
}

interval interval::expand(double delta) const
{
    auto padding = delta / 2;
    return interval(min - padding, max + padding);
}


const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);


interval operator+(const interval& ival, double displacement)
{
    return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval& ival)
{
    return ival + displacement;
}

interval operator*(const interval& ival, double displacement)
{
    return interval(ival.min * displacement, ival.max * displacement);
}

interval operator*(double displacement, const interval& ival)
{
    return ival * displacement;
}
