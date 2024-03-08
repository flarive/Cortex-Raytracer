#ifndef WORLDBUILDER_H
#define WORLDBUILDER_H

#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include "texture.h"
#include "bvh.h"


class worldbuilder
{
public:

    hittable_list random_spheres(camera &cam)
    {
        hittable_list world;

        //auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
        //world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

        auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
        world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));


        for (int a = -11; a < 11; a++)
        {
            for (int b = -11; b < 11; b++)
            {
                auto choose_mat = random_double();
                point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

                if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                    shared_ptr<material> sphere_material;

                    if (choose_mat < 0.8)
                    {
                        // diffuse
                        auto albedo = color::random() * color::random();
                        sphere_material = make_shared<lambertian>(albedo);
                        
                        // moving speheres
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

        auto checker = make_shared<checker_texture>(0.8, color(.2, .3, .1), color(.9, .9, .9));

        world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
        world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));


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
        
        auto earth_texture = make_shared<image_texture>("earthmap.jpg");
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

        auto pertext = make_shared<noise_texture>(4);
        world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
        world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

        cam.vfov = 20;
        cam.lookfrom = point3(13, 2, 3);
        cam.lookat = point3(0, 0, 0);
        cam.vup = vec3(0, 1, 0);

        cam.defocus_angle = 0;

        return world;
    }



    hittable_list build2()
    {
        hittable_list world;

        // another world
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
		auto R = cos(pi / 4);

		auto material_left = make_shared<lambertian>(color(0, 0, 1));
		auto material_right = make_shared<lambertian>(color(1, 0, 0));

		world.add(make_shared<sphere>(point3(-R, 0, -1), R, material_left));
		world.add(make_shared<sphere>(point3(R, 0, -1), R, material_right));

        return world;
    }


private:

};

#endif