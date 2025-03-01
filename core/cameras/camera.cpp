#include "camera.h"

#include "../constants.h"
#include "../pdf/hittable_pdf.h"
#include "../pdf/mixture_pdf.h"
#include "../misc/hit_record.h"
#include "../misc/scatter_record.h"
#include "../misc/singleton.h"
#include "../utilities/math_utils.h"
#include "../utilities/interval.h"
#include "../samplers/sampler.h"
#include "../textures/solid_color_texture.h"
#include "../textures/image_texture.h"

camera::camera()
{
}

const int camera::getImageHeight() const
{
	return image_height;
}

const int camera::getImageWidth() const
{
	return image_width;
}

const int camera::getSqrtSpp() const
{
	return sqrt_spp;
}

const int camera::getMaxDepth() const
{
	return max_depth;
}

const int camera::getSamplePerPixel() const
{
	return samples_per_pixel;
}

const vector3 camera::get_pixel_delta_u() const
{
	return pixel_delta_u;
}

const vector3 camera::get_pixel_delta_v() const
{
	return pixel_delta_v;
}

void camera::initialize(const renderParameters& params)
{
}

const ray camera::get_ray(int i, int j, int s_i, int s_j, std::shared_ptr<sampler> aa_sampler, randomizer& rnd) const
{
    return ray{};
}




/// <summary>
/// Recursive version
/// </summary>
color camera::ray_color(const ray& r, int depth, scene& _scene, randomizer& rnd)
{
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
    {
        // return background solid color
        return background_color;
    }

    auto unit_dir = unit_vector(r.direction());

    // If the ray hits nothing, return the background color.
    // 0.001 is to fix shadow acne interval
    if (!_scene.get_world().hit(r, interval(SHADOW_ACNE_FIX, infinity), rec, depth, rnd))
    {
        if (background_texture)
        {
            return get_background_image_color(r.x, r.y, unit_dir, background_texture, background_iskybox);
        }
        else
        {
            return background_color;
        }
    }

    // ray hit a world object
    scatter_record srec;
    color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.hit_point);

    // hack for invisible primitives (such as lights)
    if (color_from_emission.a() == 0.0)
    {
        // rethrow a new ray
        _scene.get_world().hit(r, interval(rec.t + 0.001, infinity), rec, depth, rnd);
    }

    if (!rec.mat->scatter(r, _scene.get_emissive_objects(), rec, srec, rnd))
    {
        return color_from_emission;
    }

    if (_scene.get_emissive_objects().objects.size() == 0)
    {
        // no lights
        // no importance sampling
        return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, _scene, rnd);
    }

    // no importance sampling
    if (srec.skip_pdf)
        return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, _scene, rnd);

    auto light_ptr = std::make_shared<hittable_pdf>(_scene.get_emissive_objects(), rec.hit_point);

    mixture_pdf p;

    if (background_texture && background_iskybox)
    {
        mixture_pdf p_objs(light_ptr, srec.pdf_ptr, 0.5);
        p = mixture_pdf(std::make_shared<mixture_pdf>(p_objs), background_pdf, 0.8);
    }
    else
    {
        p = mixture_pdf(light_ptr, srec.pdf_ptr);
    }

    ray scattered = ray(rec.hit_point, p.generate(srec, rnd), r.time());
    double pdf_val = p.value(scattered.direction(), rnd);
    double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

    color final_color;

    if (background_texture)
    {
        // with background image
        bool double_sided = false;
        if (rec.mat->has_alpha_texture(double_sided))
        {
            // render transparent object (having an alpha texture)
            color background_behind = rec.mat->get_diffuse_pixel_color(rec);

            ray ray_behind(rec.hit_point, r.direction(), r.x, r.y, r.time());
            color background_infrontof = ray_color(ray_behind, depth - 1, _scene, rnd);

            hit_record rec_behind;
            if (_scene.get_world().hit(ray_behind, interval(0.001, infinity), rec_behind, depth, rnd))
            {
                // another object is behind the alpha textured object, display it behind
                scatter_record srec_behind;

                if (double_sided)
                {
                    if (rec_behind.mat->scatter(ray_behind, _scene.get_emissive_objects(), rec_behind, srec_behind, rnd))
                    {
                        final_color = color::blend_colors(background_behind, background_infrontof, srec.alpha_value);
                    }
                }
                else
                {
                    if (rec_behind.mat->scatter(ray_behind, _scene.get_emissive_objects(), rec_behind, srec_behind, rnd) && rec.front_face)
                    {
                        final_color = color::blend_colors(background_behind, background_infrontof, srec.alpha_value);
                    }
                    else
                    {
                        final_color = background_infrontof;
                    }
                }
            }
            else
            {
                // no other object behind the alpha textured object, just display background image
                if (double_sided)
                {
                    final_color = color::blend_colors(color_from_emission + background_behind, ray_color(ray(rec.hit_point, r.direction(), r.x, r.y, r.time()), depth - 1, _scene, rnd), srec.alpha_value);
                }
                else
                {
                    final_color = get_background_image_color(r.x, r.y, unit_dir, background_texture, background_iskybox);
                }
            }
        }
        else
        {
            // render opaque object
            color color_from_scatter = ray_color(scattered, depth - 1, _scene, rnd) / pdf_val;
            final_color = color_from_emission + srec.attenuation * scattering_pdf * color_from_scatter;
        }
    }
    else
    {
        // with background color
        color sample_color = ray_color(scattered, depth - 1, _scene, rnd);
        color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_val;

        bool double_sided = false;
        if (rec.mat->has_alpha_texture(double_sided))
        {
            // render transparent object (having an alpha texture)
            final_color = color::blend_colors(color_from_emission + color_from_scatter, ray_color(ray(rec.hit_point, r.direction(), r.x, r.y, r.time()), depth - 1, _scene, rnd), srec.alpha_value);
        }
        else
        {
            // render opaque object
            final_color = color_from_emission + color_from_scatter;
        }
    }

    return final_color;
}


point3 camera::defocus_disk_sample(randomizer& rnd) const
{
	// Returns a random point in the camera defocus disk.
	auto p = rnd.get_in_unit_disk();
	return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
}


vector3 camera::direction_from(const point3& light_pos, const point3& hit_point) const
{
	// Calculate the direction from the hit point to the light source.
	return unit_vector(light_pos - hit_point);
}

color camera::get_background_image_color(int x, int y, const vector3& unit_dir, std::shared_ptr<image_texture> background_texture, bool background_iskybox)
{
	double u, v;

	if (background_iskybox)
        get_spherical_uv(unit_dir, background_texture->getWidth(), background_texture->getHeight(), getImageWidth(), getImageHeight(), u, v);
	else
		get_screen_uv(x, y, background_texture->getWidth(), background_texture->getHeight(), getImageWidth(), getImageHeight(), u, v);

	return background_texture->value(u, v, unit_dir);
}