#pragma once

#include "../misc/color.h"
#include "../utilities/types.h"
#include "../primitives/hittable.h"
#include "../aabb.h"
#include "../materials/material.h"

/// <summary>
/// Abstract class for lights
/// </summary>
class light : public hittable
{
public:
    virtual ~light() = default;

    double getIntensity() const;
    color getColor() const;
    virtual point3 getPosition() const;



private:
    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override;


protected:
    aabb bbox; // bounding box
    std::shared_ptr<material> mat;
    double intensity;
    bool invisible = true;
    color c;
};