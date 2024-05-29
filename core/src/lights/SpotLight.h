#pragma once

#include "light.h"
#include "../misc/color.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../utilities/interval.h"
#include "../utilities/types.h"
#include "../utilities/math_utils.h"

#include <cmath>

class SpotLight
{
public:
    SpotLight(const point3& position, const vector3& direction, double intensity, double angle)
        : position(position), direction(randomizer::unit_vector(direction)), intensity(intensity), cutoffAngle(degrees_to_radians(angle))
    {
    }

    // Method to calculate the lighting contribution at a given point
    color computeLighting(const vector3& point, const vector3& normal) const
    {
        vector3 lightDir = randomizer::unit_vector(position - point);
        double cosAngle = dot(lightDir, direction);

        if (cosAngle < std::cos(cutoffAngle)) {
            return color(0, 0, 0);  // No light contribution outside the spotlight cone
        }

        double diff = std::max(dot(normal, lightDir), 0.0);
        vector3 diffuse = intensity * diff * lightDir;  // Ensure the color is scaled correctly by the intensity

        return color(diffuse.x, diffuse.y, diffuse.z);
    }

    point3 position;
    vector3 direction;
    vector3 intensity;
    double cutoffAngle;  // in radians
};

