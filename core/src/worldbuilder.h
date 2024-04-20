#pragma once

#include "primitives/hittable.h"
#include "primitives/hittable_list.h"
#include "cameras/target_camera.h"


class worldbuilder
{
public:
    scene random_spheres(target_camera& cam);
    scene two_spheres(target_camera& cam);
    scene earth(target_camera& cam);
    scene wood_sphere(target_camera& cam);
    scene two_perlin_spheres(target_camera& cam);
    scene quads(target_camera& cam);
    scene simple_light(target_camera& cam);
    scene advanced_lights(target_camera& cam);
    scene cornell_box(target_camera& cam);
    scene cornell_box_custom(target_camera& cam);
    scene cornell_box_smoke(target_camera& cam);
    scene cornell_box_phong(target_camera& cam);
    scene final_scene(target_camera& cam);
    scene cow_scene(target_camera& cam);
    scene all_materials_spheres(target_camera& cam);
    scene lambertian_spheres(target_camera& cam);
    scene phong_spheres(target_camera& cam);
    scene oren_nayar_spheres(target_camera& cam);
    scene isotropic_anisotropic_spheres(target_camera& cam);
    scene gradient_texture_demo(target_camera& cam);
    scene simple_sphere(target_camera& cam);
    scene alpha_texture_demo(target_camera& cam);
    scene extended_primitives(target_camera& cam);
    scene transparency_materials_spheres(target_camera& cam);
};