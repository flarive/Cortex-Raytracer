#pragma once

#include "light.h"
#include "../misc/color.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../utilities/interval.h"
#include "../utilities/types.h"


/// <summary>
/// Spot light
/// </summary>
class spot_light : public light
{
public:
	spot_light(point3 _position, vector3 _direction, double _cosTotalWidth, double _cosFalloffStart, double _intensity, double _radius, color _color, std::string _name = "SpotLight", bool _invisible = true);


	aabb bounding_box() const override;

	/// <summary>
	/// Logic of sphere ray hit detection
	/// </summary>
	/// <param name="r"></param>
	/// <param name="ray_t"></param>
	/// <param name="rec"></param>
	/// <returns></returns>
	bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;


	double pdf_value(const point3& o, const vector3& v) const override;


	/// <summary>
	/// Random special implementation for sphere lights (override base)
	/// </summary>
	/// <param name="origin"></param>
	/// <returns></returns>
	vector3 random(const point3& o) const override;


private:
	vector3 m_direction{};

	double m_cosTotalWidth = 0.0;
	double m_cosFalloffStart = 0.0;

	double m_radius = 0.0;
};