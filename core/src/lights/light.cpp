#include "light.h"



double light::getIntensity() const
{
    return intensity;
}

color light::getColor() const
{
    return c;
}

point3 light::getPosition() const
{
	return point3();
}


void light::updateBoundingBox()
{
    // to implement
}