#include "constants.h"
#include "color.h"
#include "camera.h"
#include "material.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "worldbuilder.h"

#include <iostream>
#include <format>
#include <chrono>


using namespace std;
using namespace std::chrono;

typedef high_resolution_clock Clock;
typedef Clock::time_point ClockTime;


void printExecutionTime(ClockTime start_time, ClockTime end_time);


int main()
{
    double sum = 0;
    double add = 1;
    
    // Create world
    hittable_list world;

    // 4 spheres scene
    //auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    //auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    //auto material_left = make_shared<dielectric>(1.5);
    //auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    //world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    //world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    //world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    //world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
    //world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

    // 2 spheres scene
    //world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    //world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));


    // 2 another 2 spheres scenes
    //auto R = cos(pi / 4);

    //auto material_left = make_shared<lambertian>(color(0, 0, 1));
    //auto material_right = make_shared<lambertian>(color(1, 0, 0));

    //world.add(make_shared<sphere>(point3(-R, 0, -1), R, material_left));
    //world.add(make_shared<sphere>(point3(R, 0, -1), R, material_right));


    // another world
    //auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    //auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    //auto material_left = make_shared<dielectric>(1.5);
    //auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    //world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    //world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    //world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    //world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
    //world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));


    worldbuilder builder;
    world = builder.build1();



    // Init camera and render world
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 256;
    cam.samples_per_pixel = 400; // antialiasing quality
    cam.max_depth = 100; // max nbr of bounces a ray can do

    cam.vfov = 20; // vertical field of view
    cam.lookfrom = point3(13,2,3); // camera position in world
    cam.lookat = point3(0, 0, 0); // camera target in world

    cam.defocus_angle = 0.6; // depth-of-field large aperture
    cam.focus_dist = 10.0; // depth-of-field large aperture


    // Start measuring time
    auto begin = Clock::now();

    cam.render(world);

    // Stop measuring time
    auto end = Clock::now();

    printExecutionTime(begin, end);
}


void printExecutionTime(ClockTime start_time, ClockTime end_time)
{
    auto execution_time_ns = duration_cast<nanoseconds>(end_time - start_time).count();
    auto execution_time_ms = duration_cast<microseconds>(end_time - start_time).count();
    auto execution_time_sec = duration_cast<seconds>(end_time - start_time).count();
    auto execution_time_min = duration_cast<minutes>(end_time - start_time).count();
    auto execution_time_hour = duration_cast<hours>(end_time - start_time).count();

    std::clog << "\nExecution Time: ";
    if (execution_time_hour > 0)
        std::clog << "" << execution_time_hour << " Hours, ";
    if (execution_time_min > 0)
        std::clog << "" << execution_time_min % 60 << " Minutes, ";
    if (execution_time_sec > 0)
        std::clog << "" << execution_time_sec % 60 << " Seconds";
}
