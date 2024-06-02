#pragma once

#include "light.h"
#include "../misc/color.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../utilities/interval.h"
#include "../utilities/types.h"


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


class spot_light2 : public light
{
public:
    spot_light2(point3 position, vector3 direction, double cutoff, double falloff, double intensity, double radius, double blur, color rgb, std::string name = "SpotLight2", bool invisible = true);

	/// <summary>
	/// Logic of sphere ray hit detection
	/// </summary>
	/// <param name="r"></param>
	/// <param name="ray_t"></param>
	/// <param name="rec"></param>
	/// <returns></returns>
	bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;

	double pdf_value(const point3& o, const vector3& v) const override;

    virtual aabb bounding_box() const override;

	vector3 random(const point3& o) const;

private:
    vector3 m_direction{};
    double m_cutoff = 0.0;
    double m_falloff = 0.0;
    double m_radius = 0.0;
    double m_blur = 0.0;
	double m_height = 0.01; // Assuming some default height

	glm::quat m_rotation;
	matrix4 m_localToWorld;
	matrix4 m_worldToLocal;

	void rotate_disk();
};