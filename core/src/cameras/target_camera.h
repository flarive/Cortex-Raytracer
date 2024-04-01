#pragma once

#include "../constants.h"
#include "../misc/color.h"
#include "../misc/ray.h"
#include "../misc/hit_record.h"
#include "../misc/scatter_record.h"
#include "../primitives/hittable.h"
#include "../primitives/hittable_list.h"
#include "../lights/light.h"
#include "../utilities/types.h"
#include "../utilities/math_utils.h"
#include "../utilities/interval.h"
#include "../renderParameters.h"
#include "../bvh_node.h"
#include "../pdf.h"
#include "../misc/scene.h"
#include "camera.h"

class target_camera : public camera
{
public:
    /* Public Camera Parameters Here */

    double  aspect_ratio = 1.0;             // Ratio of image width over height
    int     image_width = 400;              // Rendered image width in pixel count
    int     samples_per_pixel = 10;         // Count of random samples for each pixel (antialiasing)
    int     max_depth = 10;                 // Maximum number of ray bounces into scene

    double  vfov = 90;                      // Vertical view angle (field of view) (90 is for wide-angle view for example)
    point3  lookfrom = point3(0, 0, -1);    // Point camera is looking from
    point3  lookat = point3(0, 0, 0);       // Point camera is looking at
    vector3    vup = vector3(0, 1, 0);            // Camera-relative "up" direction

    // Depth of field
    double  defocus_angle = 0;              // Variation angle of rays through each pixel
    double  focus_dist = 10;                // Distance from camera lookfrom point to plane of perfect focus

    color   background;                     // Scene background color



    /// <summary>
    /// Camera render logic
    /// </summary>
    /// <param name="world"></param>
    void render(scene& _scene, const renderParameters& _params);
    
    /// <summary>
    /// Get a randomly-sampled camera ray for the pixel at location i,j, originating from the camera defocus disk,
    /// and randomly sampled around the pixel location
    /// </summary>
    /// <param name="i"></param>
    /// <param name="j"></param>
    /// <returns></returns>
    const ray get_ray(int i, int j, int s_i, int s_j);

    /// <summary>
    /// Returns a random point in the square surrounding a pixel at the origin, given the two subpixel indices (usefull for antialiasing)
    /// </summary>
    /// <returns></returns>
    vector3 pixel_sample_square(int s_i, int s_j) const;



private:
    /* Private Camera Variables Here */

    int    image_height;    // Rendered image height
    int    sqrt_spp;        // Square root of number of samples per pixel
    double recip_sqrt_spp;  // 1 / sqrt_spp
    point3 center;          // Camera center
    point3 pixel00_loc;     // Location of pixel 0, 0
    vector3   pixel_delta_u;   // Offset to pixel to the right
    vector3   pixel_delta_v;   // Offset to pixel below
    vector3   u, v, w;         // Camera frame basis vectors
    vector3   defocus_disk_u;  // Defocus disk horizontal radius
    vector3   defocus_disk_v;  // Defocus disk vertical radius

    /// <summary>
    /// Initialize camera with settings
    /// </summary>
    /// <param name="params"></param>
    void initialize(const renderParameters& params);
    
    point3 defocus_disk_sample() const;

    /// <summary>
    /// Fire a given ray and get the hit record (recursive)
    /// </summary>
    /// <param name="r"></param>
    /// <param name="world"></param>
    /// <returns></returns>
    color ray_color(const ray& r, int depth, scene& _scene);
};