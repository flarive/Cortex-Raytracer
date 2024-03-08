#include "renderParameters.h"
#include "camera.h"
#include "hittable_list.h"
#include "worldbuilder.h"
#include "timer.h"

#include <iostream>



using namespace std;


bool quietMode;





int main(int argc, char* argv[])
{
    renderParameters params = renderParameters::getArgs(argc, argv);

    // Init camera and render world
    camera cam;
    cam.aspect_ratio = params.ratio;
    cam.image_width = params.width;
    cam.samples_per_pixel = params.samplePerPixel; // antialiasing quality
    cam.max_depth = params.recursionMaxDepth; // max nbr of bounces a ray can do

    // Create world
    worldbuilder builder;
    //hittable_list world = builder.random_spheres(cam);
    //hittable_list world = builder.two_spheres(cam);
    //hittable_list world = builder.earth(cam);
    hittable_list world = builder.two_perlin_spheres(cam);
    
    // calculate bounding boxes to speed up ray computing
    world = hittable_list(make_shared<bvh_node>(world));


    timer renderTimer;

    // Start measuring time
    renderTimer.start();

    cam.render(world, params);

    // Stop measuring time
    renderTimer.stop();

    
    renderTimer.displayTime();

    exit(EXIT_SUCCESS);
}