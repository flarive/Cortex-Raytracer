#include "misc/renderParameters.h"
#include "primitives/hittable_list.h"
#include "scenes/scene_manager.h"
#include "misc/timer.h"
#include "misc/singleton.h"
#include "misc/scene.h"
#include "renderers/renderer_selector.h"
#include "randomizers/randomizer.h"

using namespace std;


bool quietMode;

Singleton* Singleton::singleton_ = nullptr;


/// <summary>
/// Main method
/// https://fr.wikipedia.org/wiki/Path_tracing
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

    randomizer rnd(DefaultRNGSeed);


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