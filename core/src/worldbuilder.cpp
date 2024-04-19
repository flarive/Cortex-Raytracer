#include "worldbuilder.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <stb/stb_image.h>

#include "primitives/hittable.h"
#include "primitives/hittable_list.h"
#include "misc/scene.h"

#include "primitives/aarect.h"
#include "primitives/box.h"
#include "primitives/sphere.h"
#include "primitives/quad.h"
#include "primitives/cylinder.h"
#include "primitives/cone.h"
#include "primitives/disk.h"
#include "primitives/torus.h"
#include "primitives/mesh.h"
#include "primitives/volume.h"

#include "primitives/translate.h"
#include "primitives/scale.h"
#include "primitives/rotate.h"

#include "lights/sphere_light.h"
#include "lights/quad_light.h"

#include "cameras/target_camera.h"

#include "materials/material.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "materials/dielectric.h"
#include "materials/phong.h"
#include "materials/oren_nayar.h"
#include "materials/isotropic.h"
#include "materials/anisotropic.h"
#include "materials/diffuse_light.h"

#include "utilities/uvmapping.h"

#include "textures/solid_color_texture.h"
#include "textures/checker_texture.h"
#include "textures/image_texture.h"
#include "textures/perlin_noise_texture.h"
#include "textures/gradient_texture.h"
#include "textures/alpha_texture.h"
#include "textures/bump_texture.h"
#include "textures/roughness_texture.h"
#include "bvh_node.h"


scene worldbuilder::random_spheres(target_camera &cam)
{
    scene world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    cam.vfov = 20; // vertical field of view
    cam.lookfrom = point3(13, 2, 3); // camera position in world
    cam.lookat = point3(0, 0, 0); // camera target in world

    cam.defocus_angle = 0.6; // depth-of-field large aperture
    cam.focus_dist = 10.0; // depth-of-field large aperture

    return world;
}

scene worldbuilder::two_spheres(target_camera&cam)
{
    scene world;

    auto checker_material = make_shared<checker_texture>(0.8, color(0,0,0), color(1,1,1));

    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker_material)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker_material)));


    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::earth(target_camera& cam)
{
    scene world;
        
    auto earth_texture = make_shared<image_texture>("../../data/textures/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    world.add(globe);

    cam.vfov = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::wood_sphere(target_camera& cam)
{
    scene world;

    auto earth_texture = make_shared<image_texture>("../../data/textures/dark_wooden_planks_diff_4k.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    world.add(globe);

    cam.vfov = 20;
    cam.lookfrom = point3(0, 0, 12);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::two_perlin_spheres(target_camera& cam)
{
    scene world;

    auto pertext = make_shared<perlin_noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    cam.vfov = 20;
    cam.lookfrom = point3(13, 2, 3);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::quads(target_camera& cam)
{
    scene world;

    // Materials
    auto left_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(point3(-3, -2, 5), vector3(0, 0, -4), vector3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2, -2, 0), vector3(4, 0, 0), vector3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3(3, -2, 1), vector3(0, 0, 4), vector3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vector3(4, 0, 0), vector3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2, -3, 5), vector3(4, 0, 0), vector3(0, 0, -4), lower_teal));


    cam.vfov = 80;
    cam.lookfrom = point3(0, 0, 9);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::simple_light(target_camera& cam)
{
    scene world;

    auto pertext = make_shared<perlin_noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
    world.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
    world.add(make_shared<quad>(point3(3, 1, -2), vector3(2, 0, 0), vector3(0, 2, 0), difflight));


    cam.vfov = 20;
    cam.lookfrom = point3(26, 3, 6);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color(0, 0, 0);

    return world;
}

scene worldbuilder::advanced_lights(target_camera& cam)
{
    scene world;

    auto pertext = make_shared<perlin_noise_texture>(4);
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    // Box
    auto red = make_shared<lambertian>(color(.65, .05, .05));
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(3, 3, 3), red);
    box1 = make_shared<raytracer::rotate>(box1, 15, 1);
    box1 = make_shared<raytracer::translate>(box1, vector3(-10, 0, 5));
    world.add(box1);

    // Light Sources
	auto light1 = make_shared<quad_light>(point3(3, 1, -2), vector3(2, 0, 0), vector3(0, 2, 0), 2, color(10, 10, 10), "QuadLight1");
    world.add(light1);

	auto light2 = make_shared<sphere_light>(point3(0, 7, 0), 1, 3, color(4, 4, 4), "SphereLight2");
	world.add(light2);
        
    cam.vfov = 26;
    cam.lookfrom = point3(26, 3, 6);
    cam.lookat = point3(0, 2, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color(0,0,0);

    return world;
}

scene worldbuilder::cornell_box(target_camera& cam)
{
    scene world;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<dielectric> glass = make_shared<dielectric>(1.5);

    // Cornell box sides
    world.add(make_shared<quad>(point3(555, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(555, 0, 0), vector3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vector3(-555, 0, 0), vector3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vector3(555, 0, 0), vector3(0, 555, 0), white));

    /// Light
    world.add(make_shared<quad>(point3(213, 554, 227), vector3(130, 0, 0), vector3(0, 0, 105), light));

    // Aluminium Box
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminum);
    box1 = make_shared<raytracer::rotate>(box1, 15, 1);
    box1 = make_shared<raytracer::translate>(box1, vector3(265, 0, 295));
    world.add(box1);

    // Glass Sphere
    world.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));


    // Light Sources
    world.add(make_shared<quad_light>(point3(343, 554, 332), vector3(-130, 0, 0), vector3(0, 0, -105), 1.5, color(15, 15, 15), "QuadLight1"));


    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color(0, 0, 0);

    return world;
}

scene worldbuilder::cornell_box_custom(target_camera& cam)
{
    scene world;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<dielectric> glass = make_shared<dielectric>(1.5);

    // Cornell box sides
    world.add(make_shared<quad>(point3(555, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(555, 0, 0), vector3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vector3(-555, 0, 0), vector3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vector3(555, 0, 0), vector3(0, 555, 0), white));

    // Aluminium Box
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminum, "AluBox");
    box1 = make_shared<raytracer::rotate>(box1, 20, 1);
    box1 = make_shared<raytracer::translate>(box1, vector3(265, 0, 295));
    world.add(box1);

    // Glass Sphere
    world.add(make_shared<sphere>(point3(190, 90, 190), 90, glass, "GlassSphere"));



    // Light Sources
    world.add(make_shared<quad_light>(point3(343, 554, 332), vector3(-130, 0, 0), vector3(0, 0, -105), 1.5, color(15, 15, 15), "QuadLight1"));
    //world.add(make_shared<sphere_light>(point3(343 - 65, 450, 332), 65, 1.0, color(4, 4, 4), "SphereLight2", false));

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color(0, 0, 0);

    return world;
}

scene worldbuilder::cornell_box_smoke(target_camera& cam)
{
    scene world;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));

    world.add(make_shared<quad>(point3(555, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 555, 0), vector3(555, 0, 0), vector3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(555, 0, 0), vector3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vector3(555, 0, 0), vector3(0, 555, 0), white));


    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<raytracer::rotate>(box1, 15, 1);
    box1 = make_shared<raytracer::translate>(box1, vector3(265, 0, 295));

    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<raytracer::rotate>(box2, -18, 1);
    box2 = make_shared<raytracer::translate>(box2, vector3(130, 0, 65));

    world.add(make_shared<volume>(box1, 0.01, color(0, 0, 0)));
    world.add(make_shared<volume>(box2, 0.01, color(1, 1, 1)));

    // Light Sources
    auto light1 = make_shared<quad_light>(point3(113, 554, 127), vector3(330, 0, 0), vector3(0, 0, 305), 1.5, color(5, 5, 5), "QuadLight1");
    world.add(light1);



    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::cornell_box_phong(target_camera& cam)
{
    scene world;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<dielectric> glass = make_shared<dielectric>(1.5);

    // Cornell box sides
    world.add(make_shared<quad>(point3(555, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), green));
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(0, 555, 0), vector3(0, 0, 555), red));
    world.add(make_shared<quad>(point3(0, 0, 0), vector3(555, 0, 0), vector3(0, 0, 555), white));
    world.add(make_shared<quad>(point3(555, 555, 555), vector3(-555, 0, 0), vector3(0, 0, -555), white));
    world.add(make_shared<quad>(point3(0, 0, 555), vector3(555, 0, 0), vector3(0, 555, 0), white));

    // Aluminium Box
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminum, "AluBox");
    box1 = make_shared<raytracer::rotate>(box1, 20, 1);
    box1 = make_shared<raytracer::translate>(box1, vector3(265, 0, 295));
    world.add(box1);

    double ambient = 0.1;
    double diffuse = 0.9;
    double specular = 0.0;
    double shininess = 0.0;


    auto phong_material = make_shared<phong>(color(0.8, 0.1, 0.2), ambient, diffuse, specular, shininess);



    // Phong Sphere
    world.add(make_shared<sphere>(point3(190, 90, 190), 90, phong_material, "PhongSphere"));



    // Light Sources
    shared_ptr<hittable> light1 = make_shared<quad_light>(point3(343, 554, 332), vector3(-130, 0, 0), vector3(0, 0, -105), 5, color(2,2,2), "QuadLight1");
    world.add(light1);

    //auto light2 = make_shared<sphere_light>(point3(343 - 65, 450, 332), 65, 1.0, color(4, 4, 4), "SphereLight2", false);
    //world.add(light2);

    cam.vfov = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color(0, 0, 0);

    return world;
}

scene worldbuilder::final_scene(target_camera& cam)
{
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    scene world;

    world.add(make_shared<bvh_node>(boxes1));

    //auto light = make_shared<diffuse_light>(color(7, 7, 7));
    //world.add(make_shared<quad>(point3(123, 554, 147), vector3(300, 0, 0), vector3(0, 0, 265), light));

    // Light Sources
    world.add(make_shared<quad_light>(point3(123, 554, 147), vector3(300, 0, 0), vector3(0, 0, 265), 1.5, color(7, 7, 7), "QuadLight1", false));



    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vector3(30, 0, 0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<volume>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<volume>(boundary, .0001, color(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("../../data/textures/earthmap.jpg"));
    world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<perlin_noise_texture>(0.1);
    world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(random(0, 165), 10, white));
    }

    world.add(make_shared<raytracer::translate>(make_shared<raytracer::rotate>(make_shared<bvh_node>(boxes2), 15, 1), vector3(-100, 270, 395)));


    cam.background = color(0, 0, 0);

    cam.vfov = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat = point3(278, 278, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::cow_scene(target_camera& cam)
{
    scene world;

    // Materials
    auto diffuseGrey = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto diffuseRed = make_shared<lambertian>(color(0.8, 0.1, 0.1));
    auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));
    auto checker_material = make_shared<checker_texture>(0.8, color(.2, .3, .1), color(.9, .9, .9));


    auto pertext = make_shared<perlin_noise_texture>(4);
    //world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, upper_orange));
    //world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));


    // Load meshes
    const std::filesystem::path& pathToMeshes = "../../data/models/";

    auto floor = make_shared<mesh>();
    mesh::loadMesh(pathToMeshes / "floor.obj", *floor);
    auto floorTransformation = glm::scale(vector3(40.0, 1.0, 40.0));
    floorTransformation = glm::translate(floorTransformation, vector3(-0.5, 0.0, -0.5));
    floor->applyTransformation(floorTransformation);
    floor->setMaterial(diffuseGrey);
    world.add(floor);

    //auto sphereDiffuseRed = make_shared<mesh>();
    //mesh::loadMesh(pathToMeshes / "smooth_sphere.obj", *sphereDiffuseRed);
    //auto sphereRedTransformation = glm::scale(vector3(1.5, 1.5, 1.5));
    //sphereRedTransformation = glm::translate(sphereRedTransformation, vector3(1.0, 1.0, 3.5));
    //sphereDiffuseRed->applyTransformation(sphereRedTransformation);
    //sphereDiffuseRed->setMaterial(diffuseRed);
    //world.add(sphereDiffuseRed);

    auto cow = make_shared<mesh>();
    mesh::loadMesh(pathToMeshes / "cow.obj", *cow);
    const auto cowTransformation = glm::translate(vector3(-2.0, 3.6, 6.0));
    cow->applyTransformation(cowTransformation);
    cow->setMaterial(diffuseRed);
    world.add(cow);

    cam.vfov = 30;
    cam.lookfrom = point3(0, 10, 25);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::extended_primitives(target_camera& cam)
{
    scene world;

    auto ground_material = make_shared<lambertian>(color(0.48, 0.83, 0.53));


    auto lambertian_material = make_shared<lambertian>(color(0.1, 0.2, 0.9));

    auto uvmapper_material = make_shared<lambertian>(make_shared<image_texture>("../../data/textures/uv_mapper_no_numbers.jpg"));


    world.add(make_shared<quad>(point3(-6, 0, 5), vector3(12, 0, 0), vector3(0, 0, -12), ground_material));

    // Cylinder
    world.add(make_shared<cylinder>(point3(-2.0, 0.0, 0.0), 0.4, 1.0, uvmapper_material, uvmapping(1.0, 1.0, 0, 0)));
    //world.add(make_shared<disk>(point3(-2.0, 0.5, 0.0), 0.4, 0.2, uvmapper_material, uvmapping(1.0, 1.0, 0, 0)));

    // Cone
    shared_ptr<hittable> cone1 = make_shared<cone>(point3(-1.0, 0.0, 0.0), 0.4, 1.0, uvmapper_material, uvmapping(1.0, 1.0, 0, 0));
    //cone1 = make_shared<raytracer::scale>(cone1, 1,1,1);
    //cone1 = make_shared<raytracer::translate>(cone1, vector3(1,0,0));
    //cone1 = make_shared<raytracer::rotate>(cone1, 45, 0);
    world.add(cone1);

    // Box
    shared_ptr<hittable> box1 = make_shared<box>(point3(0.0, 0.35, 0), point3(0.7, 0.7, 0.7), uvmapper_material, uvmapping(0.5, 0.5, 0, 0));
    //box1 = make_shared<raytracer::rotate>(box1, 30, 1);
    //box1 = make_shared<raytracer::translate>(box1, vector3(-0.5, 0, 0));
    world.add(box1);

    // Torus
    shared_ptr<hittable> torus1 = make_shared<torus>(point3(1.0, 0.4, 0.0), 0.3, 0.15, uvmapper_material, uvmapping(1.0, 1.0, 0, 0));
    //torus1 = make_shared<raytracer::scale>(torus1, 0.8);
    //torus1 = make_shared<raytracer::rotate>(torus1, 45, 0);
    //torus1 = make_shared<raytracer::translate>(torus1, vector3(0, 0.2, 2));
    world.add(torus1);

    // Sphere
    shared_ptr<hittable> sphere1 = make_shared<sphere>(point3(2.0, 0.4, 0.0), 0.4, uvmapper_material, uvmapping(1.0, 1.0, 0, 0));
    world.add(sphere1);

    // Light Sources
    world.add(make_shared<quad_light>(point3(113, 554, 127), vector3(330, 0, 0), vector3(0, 0, 305), 1.0, color(4, 4, 4), "QuadLight1"));
    //world.add(make_shared<sphere_light>(point3(0.0, 2.0, 4.0), 0.2, 6, color(4, 4, 4), "SphereLight1", false));

    cam.vfov = 18;
    cam.lookfrom = point3(0, 2, 9);
    cam.lookat = point3(0, 0.6, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color::black();

    return world;
}

scene worldbuilder::all_materials_spheres(target_camera& cam)
{
    scene world;

    auto wood_texture = make_shared<image_texture>("../../data/textures/old-wood-cracked-knots.jpg");
    auto material_ground = make_shared<lambertian>(wood_texture);
    
    auto dielectric_material = make_shared<dielectric>(1.5);
    auto metal_material = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
    auto checker_material = make_shared<checker_texture>(0.1, color(0.0, 0.0, 0.0), color(1.0, 1.0, 1.0));
    auto lambertian_material = make_shared<lambertian>(color(0.1, 0.2, 0.9));
    auto phong_material = make_shared<phong>(color(0.1, 0.8, 0.2), 0.1, 0.9, 0.0, 0.0);
    auto orennayar_material = make_shared<oren_nayar>(color(0.8, 0.5, 0.5), 0.9, 0.5);


    world.add(make_shared<quad>(point3(-6, -0.5, 0), vector3(12, 0, 0), vector3(0, 0, -12), material_ground));

    world.add(make_shared<sphere>(point3(-2.2, 0.0, -1.0), 0.5, dielectric_material));
    world.add(make_shared<sphere>(point3(-1.1, 0.0, -1.0), 0.5, lambertian_material));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, metal_material));
    world.add(make_shared<sphere>(point3(1.1, 0.0, -1.0), 0.5, phong_material));
    world.add(make_shared<sphere>(point3(2.2, 0.0, -1.0), 0.5, orennayar_material));

    //auto cylinder1 = make_shared<cylinder>(point3(0.0, -0.8, 2.0), 0.4, 0.5, make_shared<lambertian>(checker_material));
    //world.add(cylinder1);


    // Light Sources
    world.add(make_shared<quad_light>(point3(113, 554, 127), vector3(330, 0, 0), vector3(0, 0, 305), 1.6, color(4, 4, 4), "QuadLight1"));

    cam.vfov = 18;
    cam.lookfrom = point3(0, 2, 9);
    cam.lookat = point3(0, 0.6, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    cam.background = color::black();
    return world;
}


scene worldbuilder::lambertian_spheres(target_camera& cam)
{
    scene world;

    auto ground_material = make_shared<lambertian>(color(0.48, 0.83, 0.53));
    auto lambert_material1 = make_shared<lambertian>(color(1.0, 0.1, 0.1));
    auto lambert_material2 = make_shared<lambertian>(color(0.1, 1.0, 0.1));
    auto lambert_material3 = make_shared<lambertian>(color(0.1, 0.1, 1.0));
    auto lambert_material4 = make_shared<lambertian>(make_shared<image_texture>("../../data/textures/earthmap.jpg"));

    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, ground_material));

    world.add(make_shared<sphere>(point3(-1.1, 0.0, -1.0), 0.5, lambert_material1));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, lambert_material2));
    world.add(make_shared<sphere>(point3(1.1, 0.0, -1.0), 0.5, lambert_material3));
    world.add(make_shared<sphere>(point3(2.2, 0.0, -1.0), 0.5, lambert_material4));

    // Light Sources
    world.add(make_shared<quad_light>(point3(113, 554, 127), vector3(330, 0, 0), vector3(0, 0, 305), 3, color(4, 4, 4), "QuadLight1"));
    //world.add(make_shared<sphere_light>(point3(0.0, 2.0, 4.0), 0.2, 3, color(4, 4, 4), "SphereLight1"));

    //cam.background = color(0, 0, 0);

    cam.vfov = 18;
    cam.lookfrom = point3(0, 2, 9);
    cam.lookat = point3(0, 0.6, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::phong_spheres(target_camera& cam)
{
    scene world;
    
    auto ground_material = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    auto phong_material1 = make_shared<phong>(color(1.0, 0.1, 0.1), 0.1, 0.2, 0.05, 0.0);
    auto phong_material2 = make_shared<phong>(make_shared<solid_color_texture>(color(0.1, 1.0, 0.1)), 0.1, 0.5, 0.025, 0.5);
    auto phong_material3 = make_shared<phong>(color(0.1, 0.1, 1.0), 0.1, 0.9, 0.0, 1.0);
    auto phong_material4 = make_shared<phong>(make_shared<image_texture>("../../data/textures/earthmap.jpg"), 0.1, 0.5, 0.025, 0.5);

    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, ground_material));

    world.add(make_shared<sphere>(point3(-1.1, 0.0, -1.0), 0.5, phong_material1));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, phong_material2));
    world.add(make_shared<sphere>(point3(1.1, 0.0, -1.0), 0.5, phong_material3));
    world.add(make_shared<sphere>(point3(2.2, 0.0, -1.0), 0.5, phong_material4));

    // Light Sources
    
    world.add(make_shared<quad_light>(point3(113, 554, 127), vector3(330, 0, 0), vector3(0, 0, 305), 1.5, color(4, 4, 4), "QuadLight1"));
    //world.add(make_shared<sphere_light>(point3(0.0, 2.0, 4.0), 0.2, 3, color(4, 4, 4), "SphereLight1"));

    //cam.background = color(0, 0, 0);

    cam.vfov = 18;
    cam.lookfrom = point3(0, 2, 9);
    cam.lookat = point3(0, 0.6, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::oren_nayar_spheres(target_camera& cam)
{
    scene world;

    auto ground_material = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    auto oren_nayar_material1 = make_shared<oren_nayar>(color(0.4, 0.2, 1.0), 0.1, 0.0);
    auto oren_nayar_material2 = make_shared<oren_nayar>(color(0.4, 0.2, 1.0), 0.5, 0.5);
    auto oren_nayar_material3 = make_shared<oren_nayar>(color(0.4, 0.2, 1.0), 0.9, 1.0);
    auto oren_nayar_material4 = make_shared<oren_nayar>(make_shared<image_texture>("../../data/textures/earthmap.jpg"), 0.9, 1.0);



    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, ground_material));

    world.add(make_shared<sphere>(point3(-1.1, 0.0, -1.0), 0.5, oren_nayar_material1));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, oren_nayar_material2));
    world.add(make_shared<sphere>(point3(1.1, 0.0, -1.0), 0.5, oren_nayar_material3));
    world.add(make_shared<sphere>(point3(2.2, 0.0, -1.0), 0.5, oren_nayar_material4));



    // Light Sources
    world.add(make_shared<quad_light>(point3(113, 554, 127), vector3(330, 0, 0), vector3(0, 0, 305), 2.2, color(0.9, 0.9, 0.9), "QuadLight1"));
    //world.add(make_shared<sphere_light>(point3(0.0, 2.0, 4.0), 0.2, 8, color(4, 4, 4), "SphereLight1", false));



    //cam.background = color(0, 0, 0);

    cam.vfov = 18;
    cam.lookfrom = point3(0, 2, 9);
    cam.lookat = point3(0, 0.6, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}


scene worldbuilder::isotropic_anisotropic_spheres(target_camera& cam)
{
    scene world;

    auto ground_material = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    auto isotropic_material1 = make_shared<isotropic>(color(0.4, 0.2, 1.0));
    auto isotropic_material2 = make_shared<isotropic>(color(0.1, 0.2, 0.9));
    auto isotropic_material3 = make_shared<isotropic>(make_shared<image_texture>("../../data/textures/shiny-aluminium.jpg"));
    
    auto anisotropic_material1 = make_shared<anisotropic>(color(0.4, 0.2, 1.0), 2.0);
    auto anisotropic_material2 = make_shared<anisotropic>(color(0.1, 0.2, 0.9), 10.0);
    auto anisotropic_material3 = make_shared<anisotropic>(make_shared<image_texture>("../../data/textures/shiny-aluminium.jpg"), 5.0);


    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, ground_material));

    world.add(make_shared<sphere>(point3(-2.2, 0.0, -1.0), 0.5, isotropic_material1));
    world.add(make_shared<sphere>(point3(-1.1, 0.0, -1.0), 0.5, isotropic_material2));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, isotropic_material3));
    world.add(make_shared<sphere>(point3(1.1, 0.0, -1.0), 0.5, anisotropic_material1));
    world.add(make_shared<sphere>(point3(2.2, 0.0, -1.0), 0.5, anisotropic_material2));
    world.add(make_shared<sphere>(point3(3.3, 0.0, -1.0), 0.5, anisotropic_material3));



    // Light Sources
    world.add(make_shared<quad_light>(point3(113, 554, 0), vector3(330, 0, 0), vector3(0, 0, 305), 1, color(2, 2, 2), "QuadLight1"));
    //world.add(make_shared<sphere_light>(point3(0.0, 2.0, 4.0), 0.2, 8, color(4, 4, 4), "SphereLight1", false));



    cam.background = color(0, 0, 0);

    cam.vfov = 22;
    cam.lookfrom = point3(0.5, 2, 9);
    cam.lookat = point3(0.5, 0.6, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}

scene worldbuilder::gradient_texture_demo(target_camera& cam)
{
    scene world;

	auto ground_material = make_shared<lambertian>(color(0.48, 0.83, 0.53));
	auto gradient_material = make_shared<lambertian>(make_shared<gradient_texture>(color(0, 1, 0), color(1, 0, 0), false, false));

	world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, ground_material));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, gradient_material));


	// Light Sources
    world.add(make_shared<quad_light>(point3(113, 554, 127), vector3(330, 0, 0), vector3(0, 0, 305), 1.5, color(4, 4, 4), "QuadLight1"));



	cam.vfov = 12;
	cam.lookfrom = point3(0, 2, 9);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vector3(0, 1, 0);

	cam.defocus_angle = 0;

	return world;
}

scene worldbuilder::simple_sphere(target_camera& cam)
{
    scene world;

    auto solid_material = make_shared<lambertian>(make_shared<solid_color_texture>(color(0.5, 0.5, 0.7)));

    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, solid_material));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, solid_material));


    // Light Sources
    //world.add(make_shared<sphere_light>(point3(0.0, 2.0, 0.0), 0.2, 10, color(4, 4, 4), "SphereLight1", false));
    world.add(make_shared<quad_light>(point3(113, 554, -127), vector3(330, 0, 0), vector3(0, 0, 305), 1.0, color(4, 4, 4), "QuadLight1", false));



    cam.vfov = 14;
    cam.lookfrom = point3(0, 2, 9);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vector3(0, 1, 0);

    cam.defocus_angle = 0;

    return world;
}










scene worldbuilder::alpha_texture_demo(target_camera& cam)
{
    scene world;

    int width, height, bpp;


       //const string bump_text_location = "../../data/textures/Bark_007_Height.jpg";
       //unsigned char* bump_texture_data = stbi_load(bump_text_location.c_str(), &nxb, &nyb, &nnb, 0);
       //if (bump_texture_data == nullptr)
       //{
       //    return world;
       //}

       const string alpha_text_location = "../../data/textures/alpha.png";
       unsigned char* alpha_texture_data = stbi_load(alpha_text_location.c_str(), &width, &height, &bpp, 4);
       if (alpha_texture_data == nullptr)
       {
           return world;
       }

       auto ground_material = make_shared<lambertian>(color(0.48, 0.83, 0.53));
       //auto bark_material = make_shared<lambertian>(make_shared<image_texture>("../../data/textures/Bark_007_BaseColor_Fake.jpg"));
       //auto solid_material = make_shared<lambertian>(make_shared<solid_color_texture>(color(0.8, 0.1, 0.1)));

	auto my_alpha_texture = make_shared<alpha_texture>(alpha_texture_data, width, height, bpp);
    auto my_alpha_material = make_shared<lambertian>(my_alpha_texture);

    //auto my_bump_texture = make_shared<bump_texture>(bump_texture_data, nxb, nyb, nnb, 1.0, 20, 20);

    auto wood_texture = make_shared<image_texture>("../../data/textures/old-wood-cracked-knots.jpg");
    auto material_ground = make_shared<lambertian>(wood_texture);
    world.add(make_shared<quad>(point3(-6, -0.5, 0), vector3(12, 0, 0), vector3(0, 0, -12), material_ground));


	//world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, ground_material));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.74, my_alpha_material));


    // Light Sources
    world.add(make_shared<quad_light>(point3(343, 554, 332), vector3(-130, 0, 0), vector3(0, 0, -105), 8, color(4, 4, 4), "QuadLight1"));

	cam.vfov = 12;
	cam.lookfrom = point3(0, 2, 9);
	cam.lookat = point3(0, 0, 0);
	cam.vup = vector3(0, 1, 0);

    cam.background = color(0, 0, 0);

	cam.defocus_angle = 0;

	return world;
}