#ifndef WORLDBUILDER_H
#define WORLDBUILDER_H

#include "primitives/hittable.h"
#include "primitives/hittable_list.h"

#include "primitives/sphere.h"
#include "primitives/quad.h"
#include "primitives/cylinder.h"
#include "primitives/constant_medium.h"

#include "materials/material.h"
#include "materials/lambertian.h"
#include "materials/metal.h"
#include "materials/dielectric.h"
#include "materials/diffuse_light.h"

#include "textures/texture.h"
#include "textures/solid_color_texture.h"
#include "textures/checker_texture.h"
#include "textures/image_texture.h"
#include "textures/perlin_noise_texture.h"
#include "bvh.h"


class worldbuilder
{
public:

    hittable_list random_spheres(camera &cam)
    {
        hittable_list world;

        //auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
        //world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

        auto checker_material = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
        world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker_material)));


        for (int a = -11; a < 11; a++)
        {
            for (int b = -11; b < 11; b++)
            {
                auto choose_mat = random_double();
                point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

                if ((center - point3(4, 0.2, 0)).length() > 0.9)
                {
                    shared_ptr<material> sphere_material;

                    if (choose_mat < 0.8)
                    {
                        // diffuse
                        auto albedo = color::random() * color::random();
                        sphere_material = make_shared<lambertian>(albedo);
                        
                        // moving spheres
                        //auto center2 = center + vec3(0, random_double(0, .5), 0);
                        //world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));

                        // stationary sphere
                        world.add(make_shared<sphere>(center, 0.2, sphere_material));
                    }
                    else if (choose_mat < 0.95)
                    {
                        // metal
                        auto albedo = color::random(0.5, 1);
                        auto fuzz = random_double(0, 0.5);
                        sphere_material = make_shared<metal>(albedo, fuzz);
                        world.add(make_shared<sphere>(center, 0.2, sphere_material));
                    }
                    else
                    {
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


        cam.vfov = 90; // vertical field of view
        cam.lookfrom = point3(-2, 2, 1); // camera position in world
        cam.lookat = point3(0, 0, -1); // camera target in world

        cam.defocus_angle = 0.6; // depth-of-field large aperture
        cam.focus_dist = 10.0; // depth-of-field large aperture

        return world;
    }

    hittable_list two_spheres(camera &cam)
    {
        hittable_list world;

        auto checker_material = make_shared<checker_texture>(0.8, color(.2, .3, .1), color(.9, .9, .9));

        world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker_material)));
        world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker_material)));


        cam.vfov = 20;
        cam.lookfrom = point3(13, 2, 3);
        cam.lookat = point3(0, 0, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        return world;
    }

    hittable_list earth(camera& cam)
    {
        hittable_list world;
        
        auto earth_texture = make_shared<image_texture>("../../data/textures/earthmap.jpg");
        auto earth_surface = make_shared<lambertian>(earth_texture);
        auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

        world.add(globe);

        cam.vfov = 20;
        cam.lookfrom = point3(0, 0, 12);
        cam.lookat = point3(0, 0, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        return world;
    }

    hittable_list two_perlin_spheres(camera& cam)
    {
        hittable_list world;

        auto pertext = make_shared<perlin_noise_texture>(4);
        world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
        world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

        cam.vfov = 20;
        cam.lookfrom = point3(13, 2, 3);
        cam.lookat = point3(0, 0, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        return world;
    }

    hittable_list quads(camera& cam)
    {
        hittable_list world;

        // Materials
        auto left_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
        auto back_green = make_shared<lambertian>(color(0.2, 1.0, 0.2));
        auto right_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
        auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
        auto lower_teal = make_shared<lambertian>(color(0.2, 0.8, 0.8));

        // Quads
        world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0, -4), vec3(0, 4, 0), left_red));
        world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
        world.add(make_shared<quad>(point3(3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
        world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
        world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0, -4), lower_teal));


        cam.vfov = 80;
        cam.lookfrom = point3(0, 0, 9);
        cam.lookat = point3(0, 0, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        return world;
    }

    hittable_list simple_light(camera& cam)
    {
        hittable_list world;

        auto pertext = make_shared<perlin_noise_texture>(4);
        world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
        world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

        auto difflight = make_shared<diffuse_light>(color(4, 4, 4));
        world.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
        world.add(make_shared<quad>(point3(3, 1, -2), vec3(2, 0, 0), vec3(0, 2, 0), difflight));


        cam.vfov = 20;
        cam.lookfrom = point3(26, 3, 6);
        cam.lookat = point3(0, 2, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        cam.background = color(0,0,0);

        return world;
    }

    hittable_list cornell_box(camera& cam)
    {
        hittable_list world;

        auto red = make_shared<lambertian>(color(.65, .05, .05));
        auto white = make_shared<lambertian>(color(.73, .73, .73));
        auto green = make_shared<lambertian>(color(.12, .45, .15));
        auto light = make_shared<diffuse_light>(color(15, 15, 15));

        world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
        world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
        world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130, 0, 0), vec3(0, 0, -105), light));
        world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
        world.add(make_shared<quad>(point3(555, 555, 555), vec3(-555, 0, 0), vec3(0, 0, -555), white));
        world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

        shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
        box1 = make_shared<rotate_y>(box1, 15);
        box1 = make_shared<translate>(box1, vec3(265, 0, 295));
        world.add(box1);

        shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
        box2 = make_shared<rotate_y>(box2, -18);
        box2 = make_shared<translate>(box2, vec3(130, 0, 65));
        world.add(box2);

        cam.vfov = 40;
        cam.lookfrom = point3(278, 278, -800);
        cam.lookat = point3(278, 278, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        cam.background = color(0, 0, 0);

        return world;
    }

    hittable_list cornell_box_smoke(camera& cam)
    {
        hittable_list world;

        auto red = make_shared<lambertian>(color(.65, .05, .05));
        auto white = make_shared<lambertian>(color(.73, .73, .73));
        auto green = make_shared<lambertian>(color(.12, .45, .15));
        auto light = make_shared<diffuse_light>(color(7, 7, 7));

        world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), green));
        world.add(make_shared<quad>(point3(0, 0, 0), vec3(0, 555, 0), vec3(0, 0, 555), red));
        world.add(make_shared<quad>(point3(113, 554, 127), vec3(330, 0, 0), vec3(0, 0, 305), light));
        world.add(make_shared<quad>(point3(0, 555, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
        world.add(make_shared<quad>(point3(0, 0, 0), vec3(555, 0, 0), vec3(0, 0, 555), white));
        world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 555, 0), white));

        shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(165, 330, 165), white);
        box1 = make_shared<rotate_y>(box1, 15);
        box1 = make_shared<translate>(box1, vec3(265, 0, 295));

        shared_ptr<hittable> box2 = box(point3(0, 0, 0), point3(165, 165, 165), white);
        box2 = make_shared<rotate_y>(box2, -18);
        box2 = make_shared<translate>(box2, vec3(130, 0, 65));

        world.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
        world.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));


        cam.background = color(0, 0, 0);

        cam.vfov = 40;
        cam.lookfrom = point3(278, 278, -800);
        cam.lookat = point3(278, 278, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        return world;
    }

    hittable_list final_scene(camera& cam)
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

                boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
            }
        }

        hittable_list world;

        world.add(make_shared<bvh_node>(boxes1));

        auto light = make_shared<diffuse_light>(color(7, 7, 7));
        world.add(make_shared<quad>(point3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

        auto center1 = point3(400, 400, 200);
        auto center2 = center1 + vec3(30, 0, 0);
        auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
        world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

        world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
        world.add(make_shared<sphere>(
            point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
        ));

        auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
        world.add(boundary);
        world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
        boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
        world.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

        auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
        world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
        auto pertext = make_shared<perlin_noise_texture>(0.1);
        world.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

        hittable_list boxes2;
        auto white = make_shared<lambertian>(color(.73, .73, .73));
        int ns = 1000;
        for (int j = 0; j < ns; j++) {
            boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
        }

        world.add(make_shared<translate>(
            make_shared<rotate_y>(
                make_shared<bvh_node>(boxes2), 15),
            vec3(-100, 270, 395)
        )
        );

        cam.background = color(0, 0, 0);

        cam.vfov = 40;
        cam.lookfrom = point3(478, 278, -600);
        cam.lookat = point3(278, 278, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        return world;
    }


    hittable_list build2(camera& cam)
    {
        hittable_list world;

        // another world
        auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
        auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
        auto material_left = make_shared<dielectric>(1.5);
        auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);
        auto checker_material = make_shared<checker_texture>(0.1, color(0.0, 0.0, 0.0), color(1.0, 1.0, 1.0));

        world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
        world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, make_shared<lambertian>(checker_material)));
        world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
        world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
        world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

        
        auto cylinder1 = make_shared<cylinder>(point3(0.0, -0.8, 2.0), 0.4, 0.5, make_shared<lambertian>(checker_material));
        world.add(cylinder1);


        cam.vfov = 20;
        cam.lookfrom = point3(0, 2, 9);
        cam.lookat = point3(0, 0, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        return world;
    }


    hittable_list build3()
    {
        hittable_list world;

        // 4 spheres scene
        auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
        auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
        auto material_left = make_shared<dielectric>(1.5);
        auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

        world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
        world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
        world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
        world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.4, material_left));
        world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

		return world;
    }

    
    hittable_list build4()
    {
        hittable_list world;

        auto material_sphere = make_shared<lambertian>(color(0.8, 0.8, 0.0));

        // 2 spheres scene
        world.add(make_shared<sphere>(point3(0, 0, -1), 0.5, material_sphere));
        world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, material_sphere));

        return world;
    }

    hittable_list build5()
    {
        hittable_list world;

		// 2 another 2 spheres scenes
		auto R = cos(M_PI / 4);

		auto material_left = make_shared<lambertian>(color(0, 0, 1));
		auto material_right = make_shared<lambertian>(color(1, 0, 0));

		world.add(make_shared<sphere>(point3(-R, 0, -1), R, material_left));
		world.add(make_shared<sphere>(point3(R, 0, -1), R, material_right));

        return world;
    }


private:

};

#endif