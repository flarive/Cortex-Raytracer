#pragma once

#include "primitives/hittable.h"
#include "primitives/hittable_list.h"
#include "cameras/camera.h"


class worldbuilder
{
public:
    hittable_list random_spheres(camera& cam);
    hittable_list two_spheres(camera& cam);
    hittable_list earth(camera& cam);
    hittable_list wood_sphere(camera& cam);
    hittable_list two_perlin_spheres(camera& cam);
    hittable_list quads(camera& cam);
    hittable_list simple_light(camera& cam);
    hittable_list advanced_lights(camera& cam);
    hittable_list cornell_box(camera& cam);
    hittable_list cornell_box_custom(camera& cam);
    hittable_list cornell_box_smoke(camera& cam);
    hittable_list cornell_box_phong(camera& cam);
    hittable_list final_scene(camera& cam);
    hittable_list cow_scene(camera& cam);
    hittable_list three_spheres(camera& cam);
    hittable_list glossy_sphere(camera& cam);
    hittable_list lambertian_spheres(camera& cam);
    hittable_list phong_spheres(camera& cam);
    hittable_list oren_nayar_spheres(camera& cam);
    hittable_list gradient_texture_demo(camera& cam);
    hittable_list build3();
    hittable_list build4();
    hittable_list build5();
};