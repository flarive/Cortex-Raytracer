#include "renderParameters.h"
#include "cameras/camera.h"
#include "primitives/hittable_list.h"
#include "worldbuilder.h"
#include "primitives/hittable_list.h"
#include "timer.h"
#include "singleton.h"

using namespace std;


bool quietMode;

Singleton* Singleton::singleton_ = nullptr;

/**
 * Static methods should be defined outside the class.
 */
Singleton* Singleton::getInstance()
{
    /**
     * This is a safer way to create an instance. instance = new Singleton is
     * dangerous in case two instance threads wants to access at the same time
     */
   /* if (singleton_ == nullptr) {
        singleton_ = new Singleton(value);
    }*/
    return singleton_;
}


hittable_list extractLights(const hittable_list& world);

/// <summary>
/// https://github.com/Drummersbrother/raytracing-in-one-weekend
/// https://github.com/essentialblend/weekend-raytracing
/// https://github.com/EmreDogann/Software-Ray-Tracer
/// https://github.com/aleksi-kangas/raytracing
/// https://github.com/optozorax/ray-tracing?tab=readme-ov-file
/// https://github.com/lconn-dev/MinimalRT
/// https://github.com/TheVaffel/weekend-raytracer/tree/master
/// https://github.com/mgaillard
/// https://github.com/boksajak/raytracingthenextweek
/// https://github.com/NickSpyker/RayTracer
/// https://github.com/tylermorganwall/rayrender
/// https://github.com/kdridi/raytracer
/// https://github.com/define-private-public/PSRayTracing/
/// https://github.com/VladChira/ray-tracer
/// https://github.com/Friduric/ray-tracer/blob/master/src/Rendering/Materials/OrenNayarMaterial.cpp
/// https://bunjevac.net/blog/parallelizing-raytracer-in-a-weekend/
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[])
{
    renderParameters params = renderParameters::getArgs(argc, argv);

    Singleton::singleton_ = new Singleton(params);
    


    // Init camera and render world
    camera cam;
    cam.aspect_ratio = params.ratio;
    cam.image_width = params.width;
    cam.samples_per_pixel = params.samplePerPixel; // antialiasing quality
    cam.max_depth = params.recursionMaxDepth; // max nbr of bounces a ray can do
    cam.background = color(0.70, 0.80, 1.00);

    // Create world
    worldbuilder builder;
    hittable_list lights;

    //hittable_list world = builder.random_spheres(cam);
    //hittable_list world = builder.two_spheres(cam);
    //hittable_list world = builder.earth(cam);
    //hittable_list world = builder.wood_sphere(cam);
    //hittable_list world = builder.two_perlin_spheres(cam);
    //hittable_list world = builder.quads(cam);
    //hittable_list world = builder.simple_light(cam);
    //hittable_list world = builder.advanced_lights(cam);
    //hittable_list world = builder.cornell_box(cam);
    //hittable_list world = builder.cornell_box_smoke(cam);
    //hittable_list world = builder.cornell_box_custom(cam);
    //hittable_list world = builder.final_scene(cam, lights);
    //hittable_list world = builder.three_spheres(cam);
    //hittable_list world = builder.gradient_texture_demo(cam);
    //hittable_list world = builder.alpha_texture_demo(cam);
    //hittable_list world = builder.cow_scene(cam);
    hittable_list world = builder.phong_spheres(cam);
    //hittable_list world = builder.cornell_box_phong(cam);
    
    


    lights = extractLights(world);

    // calculate bounding boxes to speed up ray computing
    world = hittable_list(make_shared<bvh_node>(world));


    timer renderTimer;

    // Start measuring time
    renderTimer.start();

    cam.render(world, lights, params);

    // Stop measuring time
    renderTimer.stop();

    
    renderTimer.displayTime();

    exit(EXIT_SUCCESS);
}

hittable_list extractLights(const hittable_list& world)
{
    hittable_list lights;

	for (unsigned int i = 0; i < world.objects.size(); i++)
	{
		std::shared_ptr<light> derived = std::dynamic_pointer_cast<light>(world.objects[i]);
		if (derived)
		{
            lights.add(derived);
		}
	}

    return lights;
}