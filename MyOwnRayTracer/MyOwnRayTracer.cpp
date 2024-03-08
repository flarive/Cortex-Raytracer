#include "renderParameters.h"
#include "camera.h"
#include "hittable_list.h"
#include "worldbuilder.h"


#include <iostream>
#include <chrono>



using namespace std;
using namespace std::chrono;

typedef high_resolution_clock Clock;
typedef Clock::time_point ClockTime;

bool quietMode;



void printExecutionTime(ClockTime start_time, ClockTime end_time);


int main(int argc, char* argv[])
{
    renderParameters params = renderParameters::getArgs(argc, argv);

    
    
    // Create world
    worldbuilder builder;
    hittable_list world = builder.build2();

	// calculate bounding boxes to speed up ray computing
	world = hittable_list(make_shared<bvh_node>(world));


    // Init camera and render world
    camera cam;
    cam.aspect_ratio = params.ratio;
    cam.image_width = params.width;
    cam.samples_per_pixel = params.samplePerPixel; // antialiasing quality
    cam.max_depth = params.recursionMaxDepth; // max nbr of bounces a ray can do

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
