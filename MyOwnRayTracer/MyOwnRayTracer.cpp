#include "constants.h"
#include "renderParameters.h"
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
#include <algorithm>


using namespace std;
using namespace std::chrono;

typedef high_resolution_clock Clock;
typedef Clock::time_point ClockTime;

bool quietMode;



void printExecutionTime(ClockTime start_time, ClockTime end_time);
renderParameters getArgs(int argc, char* argv[]);


int main(int argc, char* argv[])
{
    renderParameters params = getArgs(argc, argv);
    
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





    worldbuilder builder;
    world = builder.build1();

    


    // Init camera and render world
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 256;
    cam.samples_per_pixel = 100; // antialiasing quality
    cam.max_depth = 100; // max nbr of bounces a ray can do

    cam.vfov = 90; // vertical field of view
    cam.lookfrom = point3(-2, 2, 1); // camera position in world
    cam.lookat = point3(0, 0, -1); // camera target in world

    cam.defocus_angle = 0.6; // depth-of-field large aperture
    cam.focus_dist = 10.0; // depth-of-field large aperture


    // Start measuring time
    auto begin = Clock::now();

    cam.render(world, params);

    // Stop measuring time
    auto end = Clock::now();

    printExecutionTime(begin, end);

    exit(EXIT_SUCCESS);
}

renderParameters getArgs(int argc, char* argv[])
{
    renderParameters params;
    
    int count;
    for (count = 0; count < argc; count++)
    {
        string arg = argv[count];

        if (arg.starts_with("-"))
        {
            string param = arg.substr(1);
            string value = argv[count + 1];

            if (param == "quiet")
            {
                params.quietMode = true;
            }
            else if (param == "width" && !value.empty())
            {
                params.width = stoul(value, 0, 10);
            }
            else if (param == "height" && !value.empty())
            {
                params.height = stoul(value, 0, 10);
            }
            else if (param == "ratio" && !value.empty())
            {
                double p1 = 0, p2 = 0;
                
                stringstream test(value);
                string segment;

                unsigned int loop = 0;
                while (getline(test, segment, ':'))
                {
                    if (loop == 0)
                    {
                        p1 = stoul(segment, 0, 10);
                    }
                    else if (loop == 1)
                    {
                        p2 = stoul(segment, 0, 10);
                    }

                    loop++;
                }

                if (p1 > 0 && p2 > 0)
                {
                    params.ratio = p1 / p2;
                }
            }
        }
    }

    return params;
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
