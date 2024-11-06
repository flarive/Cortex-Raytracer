// https://fr.wikipedia.org/wiki/Path_tracing

#include "misc/renderParameters.h"
#include "primitives/hittable_list.h"
#include "scenes/scene_manager.h"
#include "misc/timer.h"
#include "misc/singleton.h"
#include "misc/scene.h"
#include "renderers/renderer_selector.h"
#include "randomizers/randomize.h"
//#include "randomizers/pcg_randomizer.h"
//#include "randomizers/mersenne_twister_randomizer.h"

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


#include <random>


#include "pcg/pcg_random.hpp"


template <typename RNG>
double generateRandom(RNG& rng)
{
    return rng();  // Calls the RNG, assuming it has an operator() that returns a random number
}


/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char* argv[])
{
    std::cout << R"(
  ______    ______     _______  ___________  _______  ___  ___  
 /" _  "\  /    " \   /"      \("     _   ")/"     "||"  \/"  | 
(: ( \___)// ____  \ |:        |)__/  \\__/(: ______) \   \  /  
 \/ \    /  /    ) :)|_____/   )   \\_ /    \/    |    \\  \/   
 //  \ _(: (____/ //  //      /    |.  |    // ___)_   /\.  \   
(:   _) \\        /  |:  __   \    \:  |   (:      "| /  \   \  
 \_______)\"_____/   |__|  \___)    \__|    \_______)|___/\___| 

----------------------- CORTEX RAYTRACER ----------------------
--------------- MONTE CARLO BASED PATH TRACING ----------------

)" << '\n';


    std::cout << "[INFO] Parsing input params" << std::endl;
    
    renderParameters params = renderParameters::getArgs(argc, argv);

    Singleton::singleton_ = new Singleton(params);

    randomizer rnd(DefaultRNGSeed, params.randomizer_type);


    // Create world
    scene_manager builder;
 
    scene world = builder.load_scene(params, rnd);
    
    std::cout << "[INFO] Ready !" << std::endl;
    
    timer renderTimer;

    // Start measuring time
    renderTimer.start();

    renderer_selector render;
    render.render(world, params, rnd);

    // Stop measuring time
    renderTimer.stop();

    //if (!params.quietMode)
        renderTimer.displayTime();

    std::cout << "[INFO] Finished !" << std::endl;

    exit(EXIT_SUCCESS);
}