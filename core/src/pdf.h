#pragma once

#include "constants.h"
#include "utilities/types.h"
#include "onb.h"
#include "primitives/hittable.h"


/// <summary>
/// Probability Density Function (henceforth PDF).
/// In short, a PDF is a continuous function that can be integrated over to determine how likely a result is over an integral.
/// Remember that the PDF is a probability function.
/// </summary>
class pdf
{
public:
    virtual ~pdf() {}

    virtual double value(const vector3& direction) const = 0;
    virtual vector3 generate() const = 0;
};


class cosine_pdf : public pdf
{
public:
    cosine_pdf(const vector3& w) { uvw.build_from_w(w); }

    double value(const vector3& direction) const override;
    vector3 generate() const override;


private:
    onb uvw;
};


class sphere_pdf : public pdf
{
public:
    sphere_pdf() { }

    double value(const vector3& direction) const override;
    vector3 generate() const override;
};


class hittable_pdf : public pdf
{
public:
    hittable_pdf(const hittable& _objects, const point3& _origin)
        : objects(_objects), origin(_origin)
    {}

    double value(const vector3& direction) const override;
    vector3 generate() const override;


private:
    const hittable& objects;
    point3 origin;
};


class mixture_pdf : public pdf
{
public:
    mixture_pdf(std::shared_ptr<pdf> p0, std::shared_ptr<pdf> p1)
    {
        p[0] = p0;
        p[1] = p1;
    }

    double value(const vector3& direction) const override;
    vector3 generate() const override;


private:
    std::shared_ptr<pdf> p[2];
};