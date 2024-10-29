#include "perspective_camera.h"

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

#include <iostream>
#include <memory>



void perspective_camera::initialize(const renderParameters& params)
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

const ray perspective_camera::get_ray(int i, int j, int s_i, int s_j, std::shared_ptr<sampler> aa_sampler, randomizer& rnd) const
{
    vector3 pixel_center = pixel00_loc + (vector3(i) * pixel_delta_u) + (vector3(j) * pixel_delta_v);

    // Apply antialiasing
    vector3 pixel_sample{};
    
    if (aa_sampler)
    {
        // using given anti aliasing sampler
        pixel_sample = pixel_center + aa_sampler->generate_samples(s_i, s_j, rnd);
    }
    else
    {
        // no anti aliasing
        pixel_sample = pixel_center;
    }


    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample(rnd);
    auto ray_direction = pixel_sample - ray_origin;
    auto ray_time = rnd.get_real(0.0, 1.0); // for motion blur

    return ray(ray_origin, ray_direction, i, j, ray_time);
}