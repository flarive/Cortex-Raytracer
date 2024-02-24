#ifndef CAMERA_H
#define CAMERA_H

#include "constants.h"
#include "color.h"
#include "hittable.h"

#include <iostream>


class camera
{
public:
    /* Public Camera Parameters Here */

    double aspect_ratio = 1.0;  // Ratio of image width over height
    int image_width = 400;  // Rendered image width in pixel count
    int samples_per_pixel = 10;   // Count of random samples for each pixel (antialiasing)

    void render(const hittable& world)
    {
        initialize();

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j)
        {
            std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
            for (int i = 0; i < image_width; ++i)
            {
                color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; ++sample)
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, world);
                }
                write_color(std::cout, pixel_color, samples_per_pixel);
            }
        }

        std::clog << "\rDone.                 \n";
    }

    ray get_ray(int i, int j) const
    {
        // Get a randomly sampled camera ray for the pixel at location i,j.

        vec3 pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);

        // Apply antialiasing
        vec3 pixel_sample = pixel_center + pixel_sample_square();

        point3 ray_origin = center;
        vec3 ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 pixel_sample_square() const
    {
        // Returns a random point in the square surrounding a pixel at the origin (usefull for antialiasing)
        auto px = -0.5 + random_double();
        auto py = -0.5 + random_double();
        return (px * pixel_delta_u) + (py * pixel_delta_v);
    }


private:
    /* Private Camera Variables Here */

    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below

    void initialize()
    {
        // Calculate the image height, and ensure that it's at least 1.
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = point3(0, 0, 0);

        // Determine viewport dimensions.
        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        std::clog << "Viewport : " << viewport_width << " x " << viewport_height << "\n";

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);

        std::clog << "viewport_uv : " << viewport_u << " / " << viewport_v << "\n";

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        std::clog << "pixel_delta_uv : " << pixel_delta_u << " / " << pixel_delta_v << "\n";

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


        std::clog << "viewport_upper_left_pixel : " << viewport_upper_left << "\n";
        std::clog << "pixel00_loc : " << pixel00_loc << "\n";
    }

    /// <summary>
    /// Fire a given ray and get the hit record
    /// </summary>
    /// <param name="r"></param>
    /// <param name="world"></param>
    /// <returns></returns>
    color ray_color(const ray& r, const hittable& world)
    {
        hit_record rec;
        if (world.hit(r, interval(0, infinity), rec))
        {
            // ray hit a world object

            // get a random vector from the hit (ray bounce)
            vec3 direction = random_on_hemisphere(rec.normal);

            // return 50% of the color from a bounce
            return 0.5 * ray_color(ray(rec.p, direction), world);
        }

        // no hit, apply default vertical background gradient
        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};

#endif