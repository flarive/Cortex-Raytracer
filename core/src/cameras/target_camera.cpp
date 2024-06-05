#include "target_camera.h"

#include "../constants.h"
#include "../pdf/hittable_pdf.h"
#include "../pdf/mixture_pdf.h"
#include "../misc/hit_record.h"
#include "../misc/scatter_record.h"
#include "../utilities/math_utils.h"
#include "../utilities/randomizer.h"
#include "../utilities/interval.h"

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

const ray target_camera::get_ray(int i, int j, int s_i, int s_j) const
{
    vector3 pixel_center = pixel00_loc + (vector3(i) * pixel_delta_u) + (vector3(j) * pixel_delta_v);

    // Apply antialiasing
    auto pixel_sample = pixel_center + pixel_sample_square(s_i, s_j);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = randomizer::random_double(); // for motion blur

    return ray(ray_origin, ray_direction, ray_time);
}


vector3 target_camera::pixel_sample_square(int s_i, int s_j) const
{
    auto px = -0.5 + recip_sqrt_spp * (s_i + randomizer::random_double());
    auto py = -0.5 + recip_sqrt_spp * (s_j + randomizer::random_double());
    return (px * pixel_delta_u) + (py * pixel_delta_v);
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
            // new
            auto unit_dir = randomizer::unit_vector(r.direction());

            double u, v;
            get_spherical_uv(unit_dir, u, v);
            return background_texture->value(u, v, unit_dir);
        }
        else
        {
            // old
            return background_color;
        }
    }

    // ray hit a world object
    scatter_record srec;
    color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.hit_point);


    // hack for invisible primitives (such as lights)
    if (color_from_emission.a() == 0)
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
    {
        if (background_texture)
        {
            // new
            //https://github.com/Drummersbrother/raytracing-in-one-weekend/blob/90b1d3d7ce7f6f9244bcb925c77baed4e9d51705/main.cpp#L26
            return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, _scene, random);
        }
        else
        {
            // old
            return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, _scene, random);
        }
    }

    auto light_ptr = std::make_shared<hittable_pdf>(_scene.get_emissive_objects(), rec.hit_point);

    mixture_pdf p;

    if (background_texture)
    {
        // new
        mixture_pdf p_objs(light_ptr, srec.pdf_ptr, 0.5);
        p = mixture_pdf(std::make_shared<mixture_pdf>(p_objs), background_pdf, 0.8);
    }
    else
    {
        // old
        p = mixture_pdf(light_ptr, srec.pdf_ptr);
    }
    
    ray scattered = ray(rec.hit_point, p.generate(random, srec), r.time());
    double pdf_val = p.value(scattered.direction());
    double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);


    if (background_texture)
    {
        // new
        color color_from_scatter = ray_color(scattered, depth - 1, _scene, random) / pdf_val;
        return color_from_emission + srec.attenuation * scattering_pdf * color_from_scatter;
    }
    else
    {
        // old
        color sample_color = ray_color(scattered, depth - 1, _scene, random);
        color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_val;
        return color_from_emission + color_from_scatter;
    }
}

vector3 target_camera::direction_from(const point3& light_pos, const point3& hit_point) const
{
	// Calculate the direction from the hit point to the light source.
	return randomizer::unit_vector(light_pos - hit_point);
}