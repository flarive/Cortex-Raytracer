// https://fr.wikipedia.org/wiki/Path_tracing

#include "renderParameters.h"
#include "cameras/target_camera.h"
#include "primitives/hittable_list.h"
#include "worldbuilder.h"
#include "primitives/hittable_list.h"
#include "timer.h"
#include "singleton.h"
#include "misc/scene.h"
#include "renderer.h"

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


/// <summary>
/// https://github.com/Drummersbrother/raytracing-in-one-weekend
/// https://github.com/essentialblend/weekend-raytracing
/// https://github.com/EmreDogann/Software-Ray-Tracer
/// https://github.com/aleksi-kangas/raytracing
/// https://github.com/optozorax/ray-tracing?tab=readme-ov-file
/// https://github.com/lconn-dev/MinimalRT
/// https://github.com/TheVaffel/weekend-raytracer/tree/master
/// https://github.com/mgaillard/Renderer
/// https://github.com/boksajak/raytracingthenextweek
/// https://github.com/NickSpyker/RayTracer
/// https://github.com/tylermorganwall/rayrender
/// https://github.com/kdridi/raytracer
/// https://github.com/define-private-public/PSRayTracing/
/// https://github.com/VladChira/ray-tracer
/// https://github.com/Friduric/ray-tracer/blob/master/src/Rendering/Materials/OrenNayarMaterial.cpp
/// https://github.com/JasperJeuken/CppRayTracer
/// https://github.com/iceman201/RayTracing/blob/master/Ray%20tracing/Cone.cpp
/// https://github.com/kamiyo/RayTra/blob/master/RayTra/Torus.cpp
/// https://github.com/aromanro/RayTracer
/// 
/// CUDA
/// https://developer.nvidia.com/blog/accelerated-ray-tracing-cuda/
/// https://github.com/rogerallen/raytracinginoneweekendincuda?tab=readme-ov-file
/// Github with both CPU & GPU : https://github.com/DveloperY0115/RTFoundation

/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[])
{
    renderParameters params = renderParameters::getArgs(argc, argv);

    Singleton::singleton_ = new Singleton(params);
    


    // Init camera and render world
    target_camera cam;
    cam.aspect_ratio = params.ratio;
    cam.image_width = params.width;
    cam.samples_per_pixel = params.samplePerPixel; // antialiasing quality
    cam.max_depth = params.recursionMaxDepth; // max nbr of bounces a ray can do
    cam.background_color = color(0.70, 0.80, 1.00);

    // Create world
    worldbuilder builder;
    //hittable_list lights;

    //scene world = builder.random_spheres(cam);
    //scene world = builder.two_spheres(cam);
    //scene world = builder.earth(cam);
    //scene world = builder.wood_sphere(cam);
    //scene world = builder.two_perlin_spheres(cam);
    //scene world = builder.quads(cam);
    //scene world = builder.simple_light(cam);
    //scene world = builder.advanced_lights(cam);
    //scene world = builder.cornell_box(cam);
    //scene world = builder.cornell_box_smoke(cam);
    //scene world = builder.cornell_box_custom(cam); // 1m26s mono thread, 40s multi thread 512x512
    //scene world = builder.cornell_box_phong(cam);
    
    //scene world = builder.all_materials_spheres(cam);
    //scene world = builder.gradient_texture_demo(cam);
    //scene world = builder.alpha_texture_demo(cam);
    //scene world = builder.cow_scene(cam);
    //scene world = builder.nautilus_scene(cam);
    
    //scene world = builder.lambertian_spheres(cam);
    //scene world = builder.phong_spheres(cam);
    //scene world = builder.oren_nayar_spheres(cam);
    //scene world = builder.isotropic_anisotropic_spheres(cam);

    //scene world = builder.extended_primitives(cam);
    //scene world = builder.transparency_materials_spheres(cam);

    //scene world = builder.simple_sphere(cam);
    //scene world = builder.final_scene(cam);


    //scene world = builder.from_file_demo(cam, "../../data/scenes/lambertian_spheres.scene");
    //scene world = builder.from_file_demo(cam, "../../data/scenes/phong_spheres.scene");
    //scene world = builder.from_file_demo(cam, "../../data/scenes/orennayar_spheres.scene");
    //scene world = builder.from_file_demo(cam, "../../data/scenes/isotropic_anisotropic_spheres.scene");
    //scene world = builder.from_file_demo(cam, "../../data/scenes/all_materials_spheres.scene");
    //scene world = builder.from_file_demo(cam, "../../data/scenes/extended_primitives.scene");
    //scene world = builder.from_file_demo(cam, "../../data/scenes/transforms.scene");
    //scene world = builder.from_file_demo(cam, "../../data/scenes/all_textures_spheres.scene");

    //scene world = builder.from_file_demo(cam, "../../data/scenes/meshes.scene");


    if (params.sceneName.empty())
    {
        std::cerr << "[ERROR] No scene to render !" << std::endl;
    }

    scene world = builder.load_scene(cam, params.sceneName);

    
    std::cout << "[INFO] Ready !" << std::endl;
    
    timer renderTimer;

    // Start measuring time
    renderTimer.start();

    renderer render;
    render.render(world, cam, params);

    // Stop measuring time
    renderTimer.stop();

    if (!params.quietMode)
        renderTimer.displayTime();

    std::cout << "[INFO] Finished !" << std::endl;

    exit(EXIT_SUCCESS);
}