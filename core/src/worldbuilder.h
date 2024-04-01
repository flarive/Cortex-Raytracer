#pragma once

#include "primitives/hittable.h"
#include "primitives/hittable_list.h"
#include "cameras/camera.h"


class worldbuilder
{
public:
    scene random_spheres(camera& cam);
    scene two_spheres(camera& cam);
    scene earth(camera& cam);
    scene wood_sphere(camera& cam);
    scene two_perlin_spheres(camera& cam);
    scene quads(camera& cam);
    scene simple_light(camera& cam);
    scene advanced_lights(camera& cam);
    scene cornell_box(camera& cam);
    scene cornell_box_custom(camera& cam);
    scene cornell_box_smoke(camera& cam);
    scene cornell_box_phong(camera& cam);
    scene final_scene(camera& cam);
    scene cow_scene(camera& cam);
    scene three_spheres(camera& cam);
    scene glossy_sphere(camera& cam);
    scene lambertian_spheres(camera& cam);
    scene phong_spheres(camera& cam);
    scene oren_nayar_spheres(camera& cam);
    scene gradient_texture_demo(camera& cam);
    scene build3();
    scene build4();
    scene build5();
};