#include "target_camera.h"

#include "../constants.h"
#include "../pdf/hittable_pdf.h"
#include "../pdf/mixture_pdf.h"
#include "../misc/hit_record.h"
#include "../misc/scatter_record.h"
#include "../utilities/math_utils.h"
#include "../utilities/randomizer.h"
#include "../utilities/interval.h"
#include "../samplers/sampler.h"

#include <iostream>
#include <memory>



void target_camera::initialize(const renderParameters& params)
{
    // Calculate the image height, and ensure that it's at least 1.
    image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    if (!params.quietMode)
        std::clog << "Image : " << image_width << " x " << image_height << "\n";


    center = lookfrom;

    // Determine viewport dimensions.
    auto theta = degrees_to_radians(vfov);
    auto h = tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);


    sqrt_spp = static_cast<int>(sqrt(samples_per_pixel));
    recip_sqrt_spp = 1.0 / sqrt_spp;

    // Calculate the u, v, w unit basis vectors for the camera coordinate frame.
    w = randomizer::unit_vector(lookfrom - lookat);
    u = randomizer::unit_vector(glm::cross(vup, w));
    v = glm::cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    vector3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
    vector3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge



    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / vector3(image_width);
    pixel_delta_v = viewport_v / vector3(image_height);


    // Calculate the location of the upper left pixel.
    vector3 viewport_upper_left = center - (focus_dist * w) - viewport_u / vector3(2) - viewport_v / vector3(2);
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // Calculate the camera defocus disk basis vectors.
    double defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
}

const ray target_camera::get_ray(int i, int j, int s_i, int s_j, std::shared_ptr<sampler> aa_sampler) const
{
    vector3 pixel_center = pixel00_loc + (vector3(i) * pixel_delta_u) + (vector3(j) * pixel_delta_v);

    // Apply antialiasing
    vector3 pixel_sample{};
    
    if (aa_sampler)
    {
        // using given anti aliasing sampler
        pixel_sample = pixel_center + aa_sampler->generate_samples(s_i, s_j);
    }
    else
    {
        // no anti aliasing
        pixel_sample = pixel_center;
    }


    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = randomizer::random_double(); // for motion blur

    return ray(ray_origin, ray_direction, i, j, ray_time);
}

point3 target_camera::defocus_disk_sample() const
{
    // Returns a random point in the camera defocus disk.
    auto p = randomizer::random_in_unit_disk();
    return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
}

/// <summary>
/// Fire a given ray and get the hit record (recursive)
/// </summary>
/// <param name="r"></param>
/// <param name="world"></param>
/// <returns></returns>
color target_camera::ray_color(const ray& r, int depth, scene& _scene, randomizer& random)
{
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
    {
        // return black color
        return background_color;
    }

    // If the ray hits nothing, return the background color.
    // 0.001 is to fix shadow acne interval
    if (!_scene.get_world().hit(r, interval(SHADOW_ACNE_FIX, infinity), rec, depth))
    {
        if (background_texture)
        {
            return get_background_image_color(r, background_texture, background_iskybox);
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
        _scene.get_world().hit(r, interval(rec.t + 0.001, infinity), rec, depth);
    }

    if (!rec.mat->scatter(r, _scene.get_emissive_objects(), rec, srec, random))
    {
        return color_from_emission;
    }

    if (_scene.get_emissive_objects().objects.size() == 0)
    {
        // no lights
        // no importance sampling
        return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, _scene, random);
    }

    // no importance sampling
    if (srec.skip_pdf)
        return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, _scene, random);



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

    ray scattered = ray(rec.hit_point, p.generate(random, srec), r.time());
    double pdf_val = p.value(scattered.direction());
    double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);




    color final_color;


    if (background_texture)
    {
        if (rec.mat->has_alpha())
        {
            // try 1
            //color sample_color = ray_color(scattered, depth - 1, _scene, random);

            //// Blend with background if alpha
            //color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_val;

            //// returns a gray color instead of a transparent color (TO BE FIXED)
            //color background_infrontof = ray_color(ray(rec.hit_point, r.direction(), r.time()), depth - 1, _scene, random);

            //// returns the background image color but ignore objects in front of the background (TO BE FIXED)
            //color background_behind = get_background_image_color(r, background_texture, background_iskybox);

            //// background here should be a mix of background_infrontof and background_behind
            //color background;

            //
            //final_color = blend_colors(color_from_emission + color_from_scatter, background, srec.alpha_value);




            // try 2
            //color sample_color = ray_color(scattered, depth - 1, _scene, random);
            //color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_val;

            //// Background behind the object
            //color background_behind = get_background_image_color(r, background_texture, background_iskybox);

            //// Background in front of the object
            //color background_infrontof = ray_color(ray(rec.hit_point, r.direction(), r.time()), depth - 1, _scene, random);

            //// Mix of background_infrontof and background_behind
            //color background = blend_colors(background_infrontof, background_behind, srec.alpha_value);

            //final_color = blend_colors(color_from_emission + color_from_scatter, background, srec.alpha_value);

            

            // try 3
            color background_behind = get_background_image_color(r, background_texture, background_iskybox);

            ray ray_behind(rec.hit_point, r.direction(), r.time());

            color background_infrontof = ray_color(ray_behind, depth - 1, _scene, random);

            hit_record rec_behind;
            
            if (_scene.get_world().hit(ray_behind, interval(0.001, infinity), rec_behind, depth))
            {
                final_color = blend_colors(background_behind, background_infrontof, srec.alpha_value);
            }
            else
            {

                
                final_color = blend_colors(background_infrontof, background_behind, srec.alpha_value);
            }
        }
        else
        {
            color color_from_scatter = ray_color(scattered, depth - 1, _scene, random) / pdf_val;
            final_color = color_from_emission + srec.attenuation * scattering_pdf * color_from_scatter;
        }
    }
    else
    {
        color sample_color = ray_color(scattered, depth - 1, _scene, random);
        color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_val;

        if (rec.mat->has_alpha())
        {
            final_color = blend_colors(color_from_emission + color_from_scatter, ray_color(ray(rec.hit_point, r.direction(), r.time()), depth - 1, _scene, random), srec.alpha_value);
        }
        else
        {
            final_color = color_from_emission + color_from_scatter;
        }
    }

    return final_color;
}

vector3 target_camera::direction_from(const point3& light_pos, const point3& hit_point) const
{
	// Calculate the direction from the hit point to the light source.
	return randomizer::unit_vector(light_pos - hit_point);
}

color target_camera::lerp_colors(const color& a, const color& b, double t)
{
    return (1.0 - t) * a + t * b;
}

color target_camera::blend_colors(const color& front, const color& back, double alpha)
{
    return alpha * front + (1.0 - alpha) * back;
}

color target_camera::get_background_image_color(const ray& r, std::shared_ptr<image_texture> background_texture, bool background_iskybox)
{
    auto unit_dir = randomizer::unit_vector(r.direction());

    double u, v;

    if (background_iskybox)
        get_spherical_uv(unit_dir, u, v);
    else
        get_screen_uv(r.x, r.y, background_texture->getWidth(), background_texture->getHeight(), getImageWidth(), getImageHeight(), u, v);

    return background_texture->value(u, v, unit_dir);
}