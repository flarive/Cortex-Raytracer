#include "scene_manager.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include "../primitives/hittable.h"
#include "../primitives/hittable_list.h"
#include "../misc/scene.h"

#include "../primitives/aarect.h"
#include "../primitives/box.h"
#include "../primitives/sphere.h"
#include "../primitives/quad.h"
#include "../primitives/cylinder.h"
#include "../primitives/cone.h"
#include "../primitives/disk.h"
#include "../primitives/torus.h"
#include "../primitives/volume.h"

#include "../primitives/translate.h"
#include "../primitives/scale.h"
#include "../primitives/rotate.h"

#include "../lights/omni_light.h"
#include "../lights/directional_light.h"

#include "../cameras/perspective_camera.h"

#include "../materials/material.h"
#include "../materials/lambertian_material.h"
#include "../materials/metal_material.h"
#include "../materials/dielectric_material.h"
#include "../materials/phong_material.h"
#include "../materials/oren_nayar_material.h"
#include "../materials/isotropic_material.h"
#include "../materials/anisotropic_material.h"
#include "../materials/diffuse_light.h"

#include "../utilities/uvmapping.h"
#include "../utilities/obj_mesh_loader.h"

#include "../textures/solid_color_texture.h"
#include "../textures/checker_texture.h"
#include "../textures/image_texture.h"
#include "../textures/perlin_noise_texture.h"
#include "../textures/gradient_texture.h"
#include "../textures/alpha_texture.h"
#include "../textures/bump_texture.h"
#include "../textures/roughness_texture.h"
#include "../textures/normal_texture.h"

#include "../pdf/image_pdf.h"

#include "../misc/bvh_node.h"

#include "../misc/aabb_debug.h"

#include "scene_loader.h"
#include "scene_builder.h"


scene scene_manager::load_scene(const renderParameters& params, randomizer& rnd)
{
    scene world;

    // get data from .scene file
    scene_loader config(params.sceneName);
    scene_builder scene = config.loadSceneFromFile(rnd);
    scene::imageConfig imageCfg = scene.getImageConfig();
    scene::cameraConfig cameraCfg = scene.getCameraConfig();
    world.set(scene.getSceneObjects());

    std::shared_ptr<camera> cam = nullptr;



    if (!cameraCfg.isOrthographic)
    {
        cam = std::make_shared<perspective_camera>();
        cam->vfov = cameraCfg.fov;
    }
    else
    {
        cam = std::make_shared<orthographic_camera>();
        cam->ortho_height = cameraCfg.orthoHeight;
    }


    cam->aspect_ratio = cameraCfg.aspectRatio;
    cam->image_width = imageCfg.width;
    cam->samples_per_pixel = imageCfg.spp; // denoiser quality
    cam->max_depth = imageCfg.depth; // max nbr of bounces a ray can do
    cam->background_color = color(0.70, 0.80, 1.00);
    cam->lookfrom = cameraCfg.lookFrom;
    cam->lookat = cameraCfg.lookAt;
    cam->vup = cameraCfg.upAxis;
    cam->is_orthographic = cameraCfg.isOrthographic;

    
    // Background
    if (!imageCfg.background.filepath.empty())
    {
        auto background = std::make_shared<image_texture>(imageCfg.background.filepath);
        cam->background_texture = background;
        cam->background_iskybox = imageCfg.background.is_skybox;

        if (imageCfg.background.is_skybox)
            cam->background_pdf = std::make_shared<image_pdf>(background);
    }
    else
    {
        cam->background_color = imageCfg.background.rgb;
    }



    // command line parameters are stronger than .scene parameters
    cam->aspect_ratio = params.ratio;
    cam->image_width = params.width;
    cam->samples_per_pixel = params.samplePerPixel; // antialiasing quality
    cam->max_depth = params.recursionMaxDepth; // max nbr of bounces a ray can do


    // Depth of field
    cam->defocus_angle = cameraCfg.aperture;
    cam->focus_dist = cameraCfg.focus;

    world.set_camera(cam);

    return world;
}