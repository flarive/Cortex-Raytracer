// https://fr.wikipedia.org/wiki/Path_tracing

#include "misc/renderParameters.h"
#include "primitives/hittable_list.h"
#include "scenes/scene_manager.h"
#include "misc/timer.h"
#include "misc/singleton.h"
#include "misc/scene.h"
#include "srenderer.h"

using namespace std;


bool quietMode;

Singleton* Singleton::singleton_ = nullptr;

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
/// https://medium.com/@dbildibay/ray-tracing-adventure-part-iv-678768947371
/// https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/#computing-the-tangents-and-bitangents
/// 
/// CUDA
/// https://developer.nvidia.com/blog/accelerated-ray-tracing-cuda/
/// https://github.com/rogerallen/raytracinginoneweekendincuda?tab=readme-ov-file
/// Github with both CPU & GPU : https://github.com/DveloperY0115/RTFoundation
/// 
/// https://github.com/AdilRabbani/Rendering-Lab/blob/main/light.h
/// https://github.com/elp2/ray-tracer-challenge
/// https://github.com/AkeelMedina22/Ray-Tracer/blob/main/raytracer/light/Light.cpp
/// https://github.com/define-private-public/PSRayTracing // black speckles fix !!!!
/// 
/// https://github.com/danoli3/Multithreaded-Ray-Tracer/blob/master/wxRaytracer/raytracer/Samplers/Jittered.cpp // more samplers !
/// https://github.com/gbrsouza/RayTracer/blob/master/src/orthographic_camera.cpp
/// https://github.com/Julien-Verdun/Ray-Tracing?tab=readme-ov-file#correctiongamma

/// Unwrapping a teapot : https://www.youtube.com/watch?v=skVZQhOJq8U

/// OpenCL build : https://github.com/KhronosGroup/OpenCL-Guide/blob/main/chapters/getting_started_windows.md


/// 
/// 
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[])
{
    renderParameters params = renderParameters::getArgs(argc, argv);

    Singleton::singleton_ = new Singleton(params);


    // Init camera and render world
    //perspective_camera cam;
    //cam.aspect_ratio = params.ratio;
    //cam.image_width = params.width;
    //cam.samples_per_pixel = params.samplePerPixel; // antialiasing quality



    // Create world
    scene_manager builder;
 
    //scene world = builder.cornell_box(cam);

   
    scene world = builder.load_scene(params);




    
    std::cout << "[INFO] Ready !" << std::endl;
    
    timer renderTimer;

    // Start measuring time
    renderTimer.start();

    srenderer render;
    render.render(world, params);

    // Stop measuring time
    renderTimer.stop();

    if (!params.quietMode)
        renderTimer.displayTime();

    std::cout << "[INFO] Finished !" << std::endl;

    exit(EXIT_SUCCESS);
}