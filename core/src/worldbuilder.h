#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "primitives/hittable.h"
#include "primitives/hittable_list.h"

#include "primitives/aarect.h"
#include "primitives/box.h"
#include "primitives/sphere.h"
#include "primitives/quad.h"
#include "primitives/cylinder.h"
#include "primitives/mesh.h"
#include "primitives/volume.h"

#include "primitives/translate.h"
#include "primitives/rotate_y.h"

#include "lights/sphere_light.h"
#include "lights/quad_light.h"

#include "materials/material.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "materials/dielectric.h"
#include "materials/glossy.h"
#include "materials/phong.h"
#include "materials/oren_nayar.h"
#include "materials/diffuse_light.h"

#include "textures/solid_color_texture.h"
#include "textures/checker_texture.h"
#include "textures/image_texture.h"
#include "textures/perlin_noise_texture.h"
#include "textures/gradient_texture.h"
#include "textures/alpha_texture.h"
#include "textures/bump_texture.h"
#include "textures/roughness_texture.h"
#include "bvh_node.h"
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
    hittable_list lambertian_sphere(camera& cam);
    hittable_list phong_spheres(camera& cam);
    hittable_list gradient_texture_demo(camera& cam);
    hittable_list build3();
    hittable_list build4();
    hittable_list build5();
};