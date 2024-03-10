#include "renderParameters.h"
#include "cameras/camera.h"
#include "primitives/hittable_list.h"
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
    cam.background = color(0.70, 0.80, 1.00);

    // Create world
    worldbuilder builder;
    //hittable_list world = builder.random_spheres(cam);
    //hittable_list world = builder.two_spheres(cam);
    //hittable_list world = builder.earth(cam);
    //hittable_list world = builder.two_perlin_spheres(cam);
    //hittable_list world = builder.quads(cam);
    //hittable_list world = builder.simple_light(cam);
    //hittable_list world = builder.cornell_box(cam);
    //hittable_list world = builder.cornell_box_smoke(cam);
    //hittable_list world = builder.final_scene(cam);
    hittable_list world = builder.build2(cam);
    

    
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