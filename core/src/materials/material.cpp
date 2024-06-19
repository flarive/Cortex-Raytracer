#include "material.h"

#include "../misc/hit_record.h"
#include "../textures/image_texture.h"
#include "../textures/solid_color_texture.h"



material::material()
    : material(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse)
    : material(_diffuse, nullptr, nullptr, nullptr, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular)
    : material(_diffuse, _specular, nullptr, nullptr, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal)
    : material(_diffuse, _specular, _normal, nullptr, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal, std::shared_ptr<texture> _bump)
    : material(_diffuse, _specular, _normal, _bump, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal, std::shared_ptr<texture> _bump, std::shared_ptr<texture> _displace)
    : material(_diffuse, _specular, _normal, _bump, _displace, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal, std::shared_ptr<texture> _bump, std::shared_ptr<texture> _displace, std::shared_ptr<texture> _alpha)
    : material(_diffuse, _specular, _normal, _bump, _displace, _alpha, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal, std::shared_ptr<texture> _bump, std::shared_ptr<texture> _displace, std::shared_ptr<texture> _alpha, double transparency, double refractive_index)
    : m_diffuse_texture(_diffuse), m_specular_texture(_specular), m_normal_texture(_normal), m_bump_texture(_bump), m_displacement_texture(_displace), m_alpha_texture(_alpha), m_transparency(transparency), m_refractiveIndex(refractive_index)
{
}

bool material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const
{
    return false;
}

double material::scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const
{
    return 0;
}

color material::emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
{
    return color(0, 0, 0);
}

bool material::has_alpha() const
{
    return m_has_alpha;
}

void material::set_has_alpha(bool has_value)
{
    m_has_alpha = has_value;
}

std::shared_ptr<texture> material::get_diffuse_texture() const
{
    return m_diffuse_texture;
}

color material::get_diffuse_pixel_color(std::shared_ptr<material> mat, const hit_record& rec) const
{
	if (mat)
	{
		auto diffuse_tex = mat->get_diffuse_texture();
		if (diffuse_tex)
		{
			std::shared_ptr<solid_color_texture> derived = std::dynamic_pointer_cast<solid_color_texture>(diffuse_tex);
			if (derived)
			{
				return derived->get_color();
			}
			else
			{
				std::shared_ptr<image_texture> derived2 = std::dynamic_pointer_cast<image_texture>(diffuse_tex);
				if (derived2)
				{
					return derived2->value(rec.u, rec.v, rec.hit_point);
				}
			}
		}
	}

	return color{};
}