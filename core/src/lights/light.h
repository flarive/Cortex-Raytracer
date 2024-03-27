#ifndef LIGHT_H
#define LIGHT_H

#include "../primitives/hittable.h"


/// <summary>
/// Abstract class for lights
/// </summary>
class light : public hittable
{
public:
    virtual ~light() = default;

    double getIntensity() const
    {
        return intensity;
    }

	virtual point3 getPosition() const
	{
		return point3();
	}


private:
    /// <summary>
    /// Update the internal AABB of the mesh.
    /// Warning: run this when the mesh is updated.
    /// </summary>
    void updateBoundingBox() override
    {
        // to implement
    }


protected:
    aabb bbox; // bounding box
    std::shared_ptr<material> mat;
    double intensity;
    bool invisible = true;
    color c;
};


#endif