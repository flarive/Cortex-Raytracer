#include "camera.h"

#include <iostream>



void camera::render(const hittable_list& _world, const hittable_list& _lights, const renderParameters& _params)
{
    initialize(_params);

    // write ppm file header
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; ++j)
    {
        if (!_params.quietMode)
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;

        for (int i = 0; i < image_width; ++i)
        {
            // each pixel is black at the beginning
            color pixel_color(0, 0, 0);

            for (int s_j = 0; s_j < sqrt_spp; ++s_j)
            {
                for (int s_i = 0; s_i < sqrt_spp; ++s_i)
                {
                    ray r = get_ray(i, j, s_i, s_j);

                    // pixel color is progressively being refined
                    pixel_color += ray_color(r, max_depth, _world, _lights);
                }
            }

            // write ppm file color entry
            color::write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    if (!_params.quietMode)
        std::clog << "\rDone.                 \n";
}


const ray camera::get_ray(int i, int j, int s_i, int s_j)
{
    vector3 pixel_center = pixel00_loc + (vector3(i) * pixel_delta_u) + (vector3(j) * pixel_delta_v);

    // Apply antialiasing
    auto pixel_sample = pixel_center + pixel_sample_square(s_i, s_j);

    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = random_double(); // for motion blur

    return ray(ray_origin, ray_direction, ray_time);

}


vector3 camera::pixel_sample_square(int s_i, int s_j) const
{
    auto px = -0.5 + recip_sqrt_spp * (s_i + random_double());
    auto py = -0.5 + recip_sqrt_spp * (s_j + random_double());
    return (px * pixel_delta_u) + (py * pixel_delta_v);
}


void camera::initialize(const renderParameters& params)
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
    w = unit_vector(lookfrom - lookat);
    u = unit_vector(glm::cross(vup, w));
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

point3 camera::defocus_disk_sample() const
{
    // Returns a random point in the camera defocus disk.
    auto p = random_in_unit_disk();
    return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
}

/// <summary>
/// Fire a given ray and get the hit record (recursive)
/// </summary>
/// <param name="r"></param>
/// <param name="world"></param>
/// <returns></returns>
color camera::ray_color(const ray& r, int depth, const hittable_list& world, const hittable_list& lights)
{
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
    {
        // return black color
        return background;
    }

    // If the ray hits nothing, return the background color.
    // 0.001 is to fix shadow acne interval
    if (!world.hit(r, interval(0.001, infinity), rec, depth))
    {
        return background;
    }


    //rec.is_shadowed = isShadowed(rec, lights);

    
    


    // ray hit a world object
    scatter_record srec;
    color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.hit_point);


    // hack for invisible primitives (such as lights)
    if (color_from_emission.a() == 0)
    {
        world.hit(r, interval(rec.t + 0.001, infinity), rec, depth);
    }

    if (rec.is_shadowed)
    {
        int dd = 0;
    }

    if (!rec.mat->scatter(r, lights, rec, srec))
    {
        return color_from_emission;
    }

    if (lights.objects.size() == 0)
    {
        // no lights
        // no importance sampling
        return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, world, lights);
    }

    // no importance sampling
    if (srec.skip_pdf)
    {
        return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, world, lights);
    }

    auto light_ptr = make_shared<hittable_pdf>(lights, rec.hit_point);
    mixture_pdf p(light_ptr, srec.pdf_ptr);

    ray scattered = ray(rec.hit_point, p.generate(), r.time());
    auto pdf_val = p.value(scattered.direction());

    double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

    color sample_color = ray_color(scattered, depth - 1, world, lights);
    color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_val;

    return color_from_emission + color_from_scatter;
}