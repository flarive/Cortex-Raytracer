// MyOwnRayTracer.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//


#include "constants.h"
#include "color.h"
#include "camera.h"
#include "material.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"

#include <iostream>
#include <chrono>



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


    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left = make_shared<dielectric>(1.5);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));






    // Init camera and render world
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 512;
    cam.samples_per_pixel = 100; // antialiasing quality
    cam.max_depth = 50; // max nbr of bounces a ray can do

    cam.vfov = 20; // vertical field of view
    cam.lookfrom = point3(-2, 2, 1); // camera position in world
    cam.lookat = point3(0, 0, -1); // camera target in world
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 10.0; // depth-of-field large aperture
    cam.focus_dist = 3.4; // depth-of-field large aperture


    // Start measuring time
    auto begin = std::chrono::high_resolution_clock::now();

    cam.render(world);

    // Stop measuring time and calculate the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    //printf("Result: %.20f\n", sum);

    printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);
}




// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
