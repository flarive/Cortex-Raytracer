#include "material.h"

#include "../misc/hit_record.h"
#include "../textures/image_texture.h"
#include "../textures/solid_color_texture.h"
#include "../textures/alpha_texture.h"
#include "../textures/displacement_texture.h"


material::material()
    : material(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse)
    : material(_diffuse, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular)
    : material(_diffuse, _specular, nullptr, nullptr, nullptr, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal)
    : material(_diffuse, _specular, _normal, nullptr, nullptr, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal, std::shared_ptr<texture> _bump)
    : material(_diffuse, _specular, _normal, _bump, nullptr, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal, std::shared_ptr<texture> _bump, std::shared_ptr<texture> _displace)
    : material(_diffuse, _specular, _normal, _bump, _displace, nullptr, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal, std::shared_ptr<texture> _bump, std::shared_ptr<texture> _displace, std::shared_ptr<texture> _alpha)
    : material(_diffuse, _specular, _normal, _bump, _displace, _alpha, nullptr, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal, std::shared_ptr<texture> _bump, std::shared_ptr<texture> _displace, std::shared_ptr<texture> _alpha, std::shared_ptr<texture> _emissive)
	: material(_diffuse, _specular, _normal, _bump, _displace, _alpha, _emissive, 0, 0)
{
}

material::material(std::shared_ptr<texture> _diffuse, std::shared_ptr<texture> _specular, std::shared_ptr<texture> _normal, std::shared_ptr<texture> _bump, std::shared_ptr<texture> _displace, std::shared_ptr<texture> _alpha, std::shared_ptr<texture> _emissive, double transparency, double refractive_index)
    : m_diffuse_texture(_diffuse), m_specular_texture(_specular), m_normal_texture(_normal), m_bump_texture(_bump), m_displacement_texture(_displace), m_alpha_texture(_alpha), m_emissive_texture(_emissive), m_transparency(transparency), m_refractiveIndex(refractive_index)
{
}

bool material::scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec) const
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

bool material::has_alpha_texture(bool& double_sided) const
{
	if (m_alpha_texture)
	{
		std::shared_ptr<alpha_texture> derived1 = std::dynamic_pointer_cast<alpha_texture>(m_alpha_texture);
		if (derived1)
		{
			double_sided = derived1->is_double_sided();
		}

		return true;
	}
	
	return false;
}

bool material::has_displace_texture() const
{
	if (m_displacement_texture)
	{
		std::shared_ptr<displacement_texture> derived1 = std::dynamic_pointer_cast<displacement_texture>(m_displacement_texture);
		if (derived1)
		{
			return true;
		}
	}

	return false;
}

std::shared_ptr<texture> material::get_diffuse_texture() const
{
    return m_diffuse_texture;
}

std::shared_ptr<texture> material::get_displacement_texture() const
{
	return m_displacement_texture;
}

color material::get_diffuse_pixel_color(const hit_record& rec) const
{
	auto diffuse_tex = this->get_diffuse_texture();
	if (diffuse_tex)
	{
		std::shared_ptr<solid_color_texture> derived1 = std::dynamic_pointer_cast<solid_color_texture>(diffuse_tex);
		if (derived1)
		{
			return derived1->get_color();
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

	return color{};
}