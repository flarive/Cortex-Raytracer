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
    light(point3 _position, double _intensity, color _color, bool _invisible, std::string _name);
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
    point3 m_position{};
    std::shared_ptr<material> m_mat;
    double m_intensity = 0.0;
    bool m_invisible = true;
    color m_color{};
};