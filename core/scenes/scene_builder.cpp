#include "scene_builder.h"

#include "../primitives/box.h"
#include "../primitives/cone.h"
#include "../primitives/cylinder.h"
#include "../primitives/sphere.h"
#include "../primitives/aarect.h"

#include "../materials/dielectric_material.h"
#include "../materials/lambertian_material.h"
#include "../materials/phong_material.h"
#include "../materials/oren_nayar_material.h"
#include "../materials/diffuse_light.h"
#include "../materials/metal_material.h"
#include "../materials/isotropic_material.h"
#include "../materials/anisotropic_material.h"
#include "../materials/emissive_material.h"

#include "../textures/checker_texture.h"
#include "../textures/perlin_noise_texture.h"
#include "../textures/solid_color_texture.h"
#include "../textures/image_texture.h"
#include "../textures/normal_texture.h"
#include "../textures/gradient_texture.h"
#include "../textures/marble_texture.h"
#include "../textures/bump_texture.h"
#include "../textures/alpha_texture.h"
#include "../textures/displacement_texture.h"
#include "../textures/emissive_texture.h"

#include "../lights/directional_light.h"
#include "../lights/omni_light.h"

#include "../misc/bvh_node.h"
#include "../misc/singleton.h"

#include "../primitives/rotate.h"
#include "../primitives/translate.h"
#include "../primitives/scale.h"



#include <utility>

#include "../utilities/helpers.h"
#include "scene_factory.h"

scene_builder::scene_builder()
{
  // Default image config
  this->m_imageConfig = { 225, 400, 100, 50, color(0.0, 0.0, 0.0) };

  // Default camera config
  this->m_cameraConfig = { 16.0 / 9.0, 0.0, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, 0.0, 10.0, false, 0.0, 0.0 };
}

perspective_camera scene_builder::getCamera() const
{
    perspective_camera cam;
    cam.aspect_ratio = this->m_cameraConfig.aspectRatio;
    cam.background_color = color(0, 0, 0);
    cam.image_width = 512;
    cam.lookfrom = point3(this->m_cameraConfig.lookFrom.x, this->m_cameraConfig.lookFrom.y, this->m_cameraConfig.lookFrom.z);
    cam.lookat = point3(this->m_cameraConfig.lookAt.x, this->m_cameraConfig.lookAt.y, this->m_cameraConfig.lookAt.z);
    cam.vup = vector3(this->m_cameraConfig.upAxis.x, this->m_cameraConfig.upAxis.y, this->m_cameraConfig.upAxis.z);
    cam.vfov = this->m_cameraConfig.verticalFov;
    cam.max_depth = 50;
    cam.samples_per_pixel = 100;
    cam.defocus_angle = this->m_cameraConfig.aperture;
    cam.focus_dist = this->m_cameraConfig.focusDist;

    // this->_camera.openingTime ???????????????

    return cam;
}

hittable_list scene_builder::getSceneObjects() const
{
  return this->m_objects;
}

scene::imageConfig scene_builder::getImageConfig() const
{
  return this->m_imageConfig;
}

scene_builder& scene_builder::setImageConfig(const  scene::imageConfig &config)
{
  this->m_imageConfig = config;
  return *this;
}

scene_builder& scene_builder::setImageBackgroundConfig(const color& rgb, const std::string& filepath, bool is_skybox)
{
    scene::imageBackgroundConfig bgConfig;
    bgConfig.rgb = rgb;
    bgConfig.filepath = filepath;
    bgConfig.is_skybox = is_skybox;
    
    this->m_imageConfig.background = bgConfig;
	return *this;
}

scene_builder& scene_builder::imageSize(int width, int height)
{
  this->m_imageConfig.width = width;
  this->m_imageConfig.height = height;
  return *this;
}

scene_builder &scene_builder::imageWidth(int width)
{
  this->m_imageConfig.width = width;
  return *this;
}

scene_builder& scene_builder::imageHeight(int height)
{
  this->m_imageConfig.height = height;
  return *this;
}

scene_builder& scene_builder::imageWidthWithAspectRatio(double aspectRatio)
{
  this->m_imageConfig.width = int(double(this->m_imageConfig.height) * aspectRatio);
  return *this;
}

scene_builder& scene_builder::imageHeightWithAspectRatio(double aspectRatio)
{
  this->m_imageConfig.height = int(double(this->m_imageConfig.width) / aspectRatio);
  return *this;
}

scene_builder& scene_builder::imageDepth(int depth)
{
  this->m_imageConfig.depth = depth;
  return *this;
}

scene_builder& scene_builder::imageSamplesPerPixel(int samplesPerPixel)
{
  this->m_imageConfig.spp = samplesPerPixel;
  return *this;
}

scene_builder& scene_builder::imageOutputFilePath(std::string filepath)
{
    this->m_imageConfig.outputFilePath = filepath;
    return *this;
}

scene::cameraConfig scene_builder::getCameraConfig() const
{
    return this->m_cameraConfig;
}

scene_builder& scene_builder::setCameraConfig(const  scene::cameraConfig &config)
{
  this->m_cameraConfig = config;
  return *this;
}

scene_builder& scene_builder::cameraAspectRatio(std::string aspectRatio)
{
    double ratio = helpers::getRatio(aspectRatio.c_str());
    this->m_cameraConfig.aspectRatio = ratio;
  return *this;
}

scene_builder& scene_builder::cameraOpeningTime(double time)
{
  this->m_cameraConfig.openingTime = time;
  return *this;
}

scene_builder& scene_builder::cameraLookFrom(point3 point)
{
  this->m_cameraConfig.lookFrom = point;
  return *this;
}

scene_builder& scene_builder::cameraLookAt(point3 lookAt)
{
  this->m_cameraConfig.lookAt = lookAt;
  return *this;
}

scene_builder& scene_builder::cameraUpAxis(point3 vUp)
{
  this->m_cameraConfig.upAxis = vUp;
  return *this;
}

scene_builder& scene_builder::cameraAperture(double aperture)
{
  this->m_cameraConfig.aperture = aperture;
  return *this;
}

scene_builder& scene_builder::cameraFocus(double focus)
{
  this->m_cameraConfig.focusDist = focus;
  return *this;
}

scene_builder& scene_builder::cameraFOV(double fov)
{
  this->m_cameraConfig.verticalFov = fov;
  return *this;
}

scene_builder& scene_builder::cameraIsOrthographic(bool orthographic)
{
    this->m_cameraConfig.isOrthographic = orthographic;
    return *this;
}

scene_builder& scene_builder::cameraOrthoHeight(double height)
{
    this->m_cameraConfig.orthoHeight = height;
    return *this;
}

scene_builder& scene_builder::addSolidColorTexture(const std::string& textureName, const color& rgb)
{
  this->m_textures[textureName] = std::make_shared<solid_color_texture>(rgb);
  return *this;
}

scene_builder& scene_builder::addGradientColorTexture(const std::string& textureName, const color& color1, const color& color2, bool aligned_v, bool hsv)
{
	this->m_textures[textureName] = std::make_shared<gradient_texture>(color1, color2, aligned_v, hsv);
	return *this;
}

scene_builder& scene_builder::addCheckerTexture(const std::string& textureName, double scale, const color& oddColor, const color& evenColor)
{
	this->m_textures[textureName] = std::make_shared<checker_texture>(scale, oddColor, evenColor);
	return *this;
}

scene_builder& scene_builder::addCheckerTexture(const std::string& textureName, double scale, const std::string &oddTextureName, const std::string &evenTextureName)
{
  this->m_textures[textureName] = std::make_shared<checker_texture>(scale, fetchTexture(oddTextureName), this->fetchTexture(evenTextureName));
  return *this;
}

scene_builder& scene_builder::addImageTexture(const std::string& textureName, const std::string &filepath)
{
  this->m_textures[textureName] = std::make_shared<image_texture>(filepath);
  return *this;
}

scene_builder& scene_builder::addNormalTexture(const std::string& textureName, const std::string& filepath, double strength)
{
    auto normal_tex = std::make_shared<image_texture>(filepath);
    this->m_textures[textureName] = std::make_shared<normal_texture>(normal_tex, strength);
    return *this;
}

scene_builder& scene_builder::addDisplacementTexture(const std::string& textureName, const std::string& filepath, double strength)
{
    auto displace_tex = std::make_shared<image_texture>(filepath);
    this->m_textures[textureName] = std::make_shared<displacement_texture>(displace_tex, strength);
    return *this;
}

scene_builder& scene_builder::addNoiseTexture(const std::string& textureName, double scale)
{
  this->m_textures[textureName] = std::make_shared<perlin_noise_texture>(scale);
  return *this;
}

scene_builder& scene_builder::addMarbleTexture(const std::string& textureName, double scale)
{
	this->m_textures[textureName] = std::make_shared<marble_texture>(scale);
	return *this;
}

scene_builder& scene_builder::addBumpTexture(const std::string& textureName, const std::string& filepath, double strength)
{
    auto bump_tex = std::make_shared<image_texture>(filepath);
    this->m_textures[textureName] = std::make_shared<bump_texture>(bump_tex, strength);
    return *this;
}

scene_builder& scene_builder::addAlphaTexture(const std::string& textureName, const std::string& filepath, bool double_sided)
{
    auto alpha_tex = std::make_shared<image_texture>(filepath);
    this->m_textures[textureName] = std::make_shared<alpha_texture>(alpha_tex, double_sided);
    return *this;
}

scene_builder& scene_builder::addEmissiveTexture(const std::string& textureName, const std::string& filepath, double strength)
{
    auto emissive_tex = std::make_shared<image_texture>(filepath);
    this->m_textures[textureName] = std::make_shared<emissive_texture>(emissive_tex, strength);
    return *this;
}

scene_builder& scene_builder::addGlassMaterial(const std::string &materialName, double refraction)
{
    this->m_materials[materialName] = std::make_shared<dielectric_material>(refraction);
    return *this;
}

scene_builder& scene_builder::addGlassMaterial(const std::string& materialName, double refraction, const std::string& textureName)
{
    this->m_materials[materialName] = std::make_shared<dielectric_material>(refraction, fetchTexture(textureName));
    return *this;
}

scene_builder& scene_builder::addGlassMaterial(const std::string& materialName, double refraction, const color& rgb)
{
    auto tex = std::make_shared<solid_color_texture>(rgb);
    this->m_materials[materialName] = std::make_shared<dielectric_material>(refraction, tex);
    return *this;
}

scene_builder& scene_builder::addLambertianMaterial(const std::string& materialName, const color& rgb)
{
    this->m_materials[materialName] = std::make_shared<lambertian_material>(rgb);
    return *this;
}

scene_builder& scene_builder::addLambertianMaterial(const std::string& materialName, const std::string& textureName)
{
    this->m_materials[materialName] = std::make_shared<lambertian_material>(this->m_textures[textureName]);
    return *this;
}

scene_builder& scene_builder::addPhongMaterial(const std::string& materialName, const std::string& diffuseTextureName, const std::string& specularTextureName, std::string& normalTextureName, const std::string& bumpTextureName, std::string& displaceTextureName, std::string& alphaTextureName, std::string& emissiveTextureName, const color& ambient, double shininess)
{
    this->m_materials[materialName] = std::make_shared<phong_material>(
        fetchTexture(diffuseTextureName),
        fetchTexture(specularTextureName),
        fetchTexture(bumpTextureName),
        fetchTexture(normalTextureName),
        fetchTexture(displaceTextureName),
        fetchTexture(alphaTextureName),
        fetchTexture(emissiveTextureName),
        ambient, shininess);
    return *this;
}

scene_builder& scene_builder::addOrenNayarMaterial(const std::string& materialName, const color& rgb, double albedo_temp, double roughness)
{
	this->m_materials[materialName] = std::make_shared<oren_nayar_material>(rgb, albedo_temp, roughness);
	return *this;
}

scene_builder& scene_builder::addOrenNayarMaterial(const std::string& materialName, const std::string& textureName, double albedo_temp, double roughness)
{
	this->m_materials[materialName] = std::make_shared<oren_nayar_material>(fetchTexture(textureName), albedo_temp, roughness);
	return *this;
}

scene_builder& scene_builder::addIsotropicMaterial(const std::string& materialName, const color& rgb)
{
    this->m_materials[materialName] = std::make_shared<isotropic_material>(rgb);
    return *this;
}

scene_builder& scene_builder::addIsotropicMaterial(const std::string& materialName, const std::string& textureName)
{
    this->m_materials[materialName] = std::make_shared<isotropic_material>(fetchTexture(textureName));
    return *this;
}

scene_builder& scene_builder::addAnisotropicMaterial(const std::string& materialName, double nu, double nv, const color& rgb)
{
    auto diffuse_tex = std::make_shared<solid_color_texture>(rgb);
    this->m_materials[materialName] = std::make_shared<anisotropic_material>(nu, nv, diffuse_tex, nullptr, nullptr);
    return *this;
}

scene_builder& scene_builder::addAnisotropicMaterial(const std::string& materialName, double nu, double nv, const std::string& diffuseTextureName, const std::string& specularTextureName, const std::string& exponentTextureName)
{
    this->m_materials[materialName] = std::make_shared<anisotropic_material>(nu, nv, fetchTexture(diffuseTextureName), fetchTexture(specularTextureName), fetchTexture(exponentTextureName));
    return *this;
}

scene_builder& scene_builder::addMetalMaterial(const std::string &materialName, const color& rgb, double fuzziness)
{
    this->m_materials[materialName] = std::make_shared<metal_material>(rgb, fuzziness);
    return *this;
}

scene_builder& scene_builder::addMetalMaterial(const std::string& materialName, const std::string& textureName, double fuzziness, double specular_intensity, const color& metal_tint, double anisotropy, double fresnel_factor, double heat)
{
    this->m_materials[materialName] = std::make_shared<metal_material>(fetchTexture(textureName), fuzziness, specular_intensity, metal_tint, anisotropy, fresnel_factor, heat);
    return *this;
}

scene_builder& scene_builder::addEmissiveMaterial(const std::string& materialName, const std::string& emissiveTextureName, double intensity)
{
    this->m_materials[materialName] = std::make_shared<emissive_material>(fetchTexture(emissiveTextureName), intensity);
    return *this;
}

scene_builder& scene_builder::addEmissiveMaterial(const std::string& materialName, const color& rgb, double intensity)
{
    this->m_materials[materialName] = std::make_shared<emissive_material>(rgb, intensity);
    return *this;
}

scene_builder& scene_builder::addDirectionalLight(const point3& pos, const vector3& u, const vector3& v, double intensity, const color& rgb, bool invisible, std::string name)
{
    this->m_objects.add(
        scene_factory::createDirectionalLight(
            name,
            pos,
            u,
            v,
            intensity,
            rgb,
            invisible
        )
    );
    return *this;
}

scene_builder& scene_builder::addOmniDirectionalLight(const point3& pos, double radius, double intensity, const color& rgb, bool invisible, std::string name)
{
    this->m_objects.add(
        scene_factory::createOmniDirectionalLight(
            name,
            pos,
            radius,
            intensity,
            rgb,
            invisible
        )
    );
    return *this;
}

scene_builder& scene_builder::addSpotLight(const point3& pos, const vector3& dir, double cutoff, double falloff, double intensity, double radius, const color& rgb, bool invisible, std::string name)
{
    this->m_objects.add(
        scene_factory::createSpotLight(
            name,
            pos,
            dir,
            cutoff,
            falloff,
            intensity,
            radius,
            rgb,
            invisible
        )
    );
    return *this;
}

scene_builder& scene_builder::addObject(const std::shared_ptr<hittable> &obj)
{
  this->m_objects.add(obj);
  return *this;
}

scene_builder& scene_builder::addSphere(std::string name, point3 pos, double radius, const std::string& materialName, const uvmapping& uv, const std::string& group)
{
    auto sphere = scene_factory::createSphere(name, pos, radius, fetchMaterial(materialName), uv);

    if (!group.empty())
    {
        auto it = this->m_groups.find(group);
        if (it != this->m_groups.end())
        {
            // add to existing group is found
            std::shared_ptr<hittable_list> grp = it->second;
            if (grp) { grp->add(sphere); }
        }
        else
        {
            // create group if not found
            this->m_groups.emplace(group, std::make_shared<hittable_list>(sphere));
        }
    }
    else
    {
        this->m_objects.add(sphere);
    }

	return *this;
}

scene_builder& scene_builder::addPlane(std::string name, const point3& p0, const point3& p1, const std::string& materialName, const uvmapping& uv, const std::string& group)
{
    auto plane = scene_factory::createPlane(name, p0, p1, fetchMaterial(materialName), uv);
    
	if (!group.empty())
	{
		auto it = this->m_groups.find(group);
		if (it != this->m_groups.end())
		{
			// add to existing group is found
			std::shared_ptr<hittable_list> grp = it->second;
			if (grp) { grp->add(plane); }
		}
		else
		{
			// create group if not found
			this->m_groups.emplace(group, std::make_shared<hittable_list>(plane));
		}
	}
	else
	{
		this->m_objects.add(plane);
	}

    return *this;
}

scene_builder& scene_builder::addQuad(std::string name, const point3& position, const vector3& u, const vector3& v, const std::string& materialName, const uvmapping& uv, const std::string& group)
{
    auto quad = scene_factory::createQuad(name, position, u, v, fetchMaterial(materialName), uv);
    
    if (!group.empty())
	{
		auto it = this->m_groups.find(group);
		if (it != this->m_groups.end())
		{
			// add to existing group is found
			std::shared_ptr<hittable_list> grp = it->second;
			if (grp) { grp->add(quad); }
		}
		else
		{
			// create group if not found
			this->m_groups.emplace(group, std::make_shared<hittable_list>(quad));
		}
	}
	else
	{
		this->m_objects.add(quad);
	}

    return *this;
}

scene_builder& scene_builder::addBox(std::string name, point3 p0, point3 p1, const std::string& materialName, const uvmapping& uv, const std::string& group)
{
    auto box = scene_factory::createBox(name, p0, p1, fetchMaterial(materialName), uv);

    if (!group.empty())
    {
        auto it = this->m_groups.find(group);

        if (it != this->m_groups.end())
        {
            // if key is found
            std::shared_ptr<hittable_list> grp = it->second;
            if (grp)
            {
                grp->add(box);
            }
        }
        else
        {
            // if key is not found
            this->m_groups.emplace(group, std::make_shared<hittable_list>(box));
        }
    }
    else
    {
        this->m_objects.add(box);
    }

    return *this;
}

scene_builder& scene_builder::addCylinder(std::string name, point3 pos, double radius, double height, const std::string & materialName, const uvmapping& uv, const std::string& group)
{
    auto cylinder = scene_factory::createCylinder(name, pos, radius, height, fetchMaterial(materialName), uv);
    
	if (!group.empty())
	{
		auto it = this->m_groups.find(group);

		if (it != this->m_groups.end())
		{
			// if key is found
			std::shared_ptr<hittable_list> grp = it->second;
			if (grp)
			{
				grp->add(cylinder);
			}
		}
		else
		{
			// if key is not found
			this->m_groups.emplace(group, std::make_shared<hittable_list>(cylinder));
		}
	}
	else
	{
		this->m_objects.add(cylinder);
	}
        
    return *this;
}

scene_builder& scene_builder::addDisk(std::string name, point3 pos, double radius, double height, const std::string& materialName, const uvmapping& uv, const std::string& group)
{
    auto disk = scene_factory::createDisk(name, pos, radius, height, fetchMaterial(materialName), uv);
    
	if (!group.empty())
	{
		auto it = this->m_groups.find(group);

		if (it != this->m_groups.end())
		{
			// if key is found
			std::shared_ptr<hittable_list> grp = it->second;
			if (grp)
			{
				grp->add(disk);
			}
		}
		else
		{
			// if key is not found
			this->m_groups.emplace(group, std::make_shared<hittable_list>(disk));
		}
	}
	else
	{
		this->m_objects.add(disk);
	}

    return *this;
}

scene_builder& scene_builder::addTorus(std::string name, point3 pos, double major_radius, double minor_radius, const std::string& materialName, const uvmapping& uv, const std::string& group)
{
    auto torus = scene_factory::createTorus(name, pos, major_radius, minor_radius, fetchMaterial(materialName), uv);

	if (!group.empty())
	{
		auto it = this->m_groups.find(group);

		if (it != this->m_groups.end())
		{
			// if key is found
			std::shared_ptr<hittable_list> grp = it->second;
			if (grp)
			{
				grp->add(torus);
			}
		}
		else
		{
			// if key is not found
			this->m_groups.emplace(group, std::make_shared<hittable_list>(torus));
		}
	}
	else
	{
		this->m_objects.add(torus);
	}
    
    return *this;
}

scene_builder& scene_builder::addCone(std::string name, point3 pos, double radius, double height, const std::string & materialName, const uvmapping& uv, const std::string& group)
{
    auto cone = scene_factory::createCone(name, pos, height, radius, fetchMaterial(materialName), uv);
    
	if (!group.empty())
	{
		auto it = this->m_groups.find(group);

		if (it != this->m_groups.end())
		{
			// if key is found
			std::shared_ptr<hittable_list> grp = it->second;
			if (grp)
			{
				grp->add(cone);
			}
		}
		else
		{
			// if key is not found
			this->m_groups.emplace(group, std::make_shared<hittable_list>(cone));
		}
	}
	else
	{
		this->m_objects.add(cone);
	}

    return *this;
}

scene_builder& scene_builder::addVolume(std::string name, std::string boundaryObjectName, double density, const std::string& textureName, const std::string& group)
{
    auto boundaryObject = this->m_objects.get(boundaryObjectName);
    if (boundaryObject)
    {
        auto volume = scene_factory::createVolume(name, boundaryObject, density, fetchTexture(textureName));

		if (!group.empty())
		{
			auto it = this->m_groups.find(group);

			if (it != this->m_groups.end())
			{
				// if key is found
				std::shared_ptr<hittable_list> grp = it->second;
				if (grp)
				{
					grp->add(volume);
				}
			}
			else
			{
				// if key is not found
				this->m_groups.emplace(group, std::make_shared<hittable_list>(volume));
			}
		}
		else
		{
			this->m_objects.add(volume);
		}

        this->m_objects.remove(boundaryObject);
    }

    return *this;
}

scene_builder& scene_builder::addVolume(std::string name, std::string boundaryObjectName, double density, const color& rgb, const std::string& group)
{
    auto boundaryObject = this->m_objects.get(boundaryObjectName);
    if (boundaryObject)
    {
        auto volume = scene_factory::createVolume(name, boundaryObject, density, rgb);

		if (!group.empty())
		{
			auto it = this->m_groups.find(group);

			if (it != this->m_groups.end())
			{
				// if key is found
				std::shared_ptr<hittable_list> grp = it->second;
				if (grp)
				{
					grp->add(volume);
				}
			}
			else
			{
				// if key is not found
				this->m_groups.emplace(group, std::make_shared<hittable_list>(volume));
			}
		}
		else
		{
			this->m_objects.add(volume);
		}

        this->m_objects.remove(boundaryObject);
    }

    return *this;
}

scene_builder& scene_builder::addObjMesh(std::string name, point3 pos, const std::string& filepath, const std::string& materialName, bool use_mtl, bool use_smoothing, const std::string& group, randomizer& rnd)
{
    auto mesh = scene_factory::createObjMesh(name, pos, filepath, fetchMaterial(materialName), use_mtl, use_smoothing, rnd);

    if (!mesh)
        return *this;

    if (!group.empty())
    {
        auto it = this->m_groups.find(group);

        if (it != this->m_groups.end())
        {
            // if key is found
            std::shared_ptr<hittable_list> grp = it->second;
            if (grp)
            {
                grp->add(mesh);
            }
        }
        else
        {
            // if key is not found
            this->m_groups.emplace(group, std::make_shared<hittable_list>(mesh));
        }
    }
    else
    {
        this->m_objects.add(mesh);
    }

	return *this;
}

scene_builder& scene_builder::addFbxMesh(std::string name, point3 pos, const std::string& filepath, bool use_fbx_cameras, bool use_fbx_lights, bool use_fbx_materials, bool use_fbx_textures, const std::string& group, randomizer& rnd)
{
    std::vector<std::shared_ptr<camera>> cameras;
    std::vector<std::shared_ptr<light>> lights;
    
    auto mesh = scene_factory::createFbxMesh(name, pos, filepath, use_fbx_cameras, use_fbx_lights, use_fbx_materials, use_fbx_textures, cameras, lights, m_cameraConfig.aspectRatio, rnd, m_materials, m_textures);

    if (!mesh)
        return *this;

    if (!group.empty())
    {
        auto it = this->m_groups.find(group);

        if (it != this->m_groups.end())
        {
            // if key is found
            std::shared_ptr<hittable_list> grp = it->second;
            if (grp)
            {
                grp->add(mesh);
            }
        }
        else
        {
            // if key is not found
            this->m_groups.emplace(group, std::make_shared<hittable_list>(mesh));
        }
    }
    else
    {
        this->m_objects.add(mesh);
    }

    if (use_fbx_cameras && cameras.size() > 0)
    {
        std::shared_ptr<camera> cam = cameras.at(0);
        if (cam)
        {
            // always try to take eventual .scene value first before fbx value
            m_cameraConfig.aspectRatio = cam->aspect_ratio;
            m_cameraConfig.isOrthographic = cam->is_orthographic || m_cameraConfig.isOrthographic;
            m_cameraConfig.lookAt = isNotZero(m_cameraConfig.lookAt) ? m_cameraConfig.lookAt : cam->lookat;
            m_cameraConfig.lookFrom = isNotZero(m_cameraConfig.lookFrom) ? m_cameraConfig.lookFrom : cam->lookfrom;
            m_cameraConfig.upAxis = isNotZero(m_cameraConfig.upAxis) ? m_cameraConfig.upAxis : cam->vup;
            
            if (m_cameraConfig.isOrthographic)
                m_cameraConfig.orthoHeight = m_cameraConfig.orthoHeight != 0 ? m_cameraConfig.orthoHeight : cam->ortho_height;
            else
                m_cameraConfig.verticalFov = m_cameraConfig.verticalFov > 0 ? m_cameraConfig.verticalFov : cam->vfov;

            // depth of field
            m_cameraConfig.focusDist = cam->focus_dist;
            m_cameraConfig.aperture = cam->defocus_angle;
        }
    }

    if (use_fbx_lights && lights.size() > 0)
    {
        for (int i = 0; i < lights.size(); i++)
        {
            m_objects.add(lights[i]);
        }
    }

    return *this;
}

scene_builder& scene_builder::addGroup(std::string name, bool& isUsed, randomizer& rnd)
{
    isUsed = false;
    
    auto it = this->m_groups.find(name);

    if (it != this->m_groups.end())
    {
        std::shared_ptr<hittable_list> group_objects = it->second;
        if (group_objects)
        {
            auto bvh_group = std::make_shared<bvh_node>(*group_objects, rnd, name);
            this->m_objects.add(bvh_group);

            isUsed = true;
        }
    }
    
    return *this;
}

scene_builder& scene_builder::translate(const vector3& vector, std::string name)
{
    if (!name.empty())
    {
        auto& found = this->m_objects.get(name);
        if (found)
        {
            found = std::make_shared<rt::translate>(found, vector);
        }
        else
        {
            // search in groups
            for (auto& group : this->m_groups)
            {
                auto& found2 = group.second->get(name);
                if (found2)
                {
                    found2 = std::make_shared<rt::translate>(found2, vector);
                    break;
                }
            }
        }
    }
    else
    {
        std::shared_ptr<hittable> back = this->m_objects.back();
        std::string n = back->getName();
        if (n == name)
        {
            this->m_objects.back() = std::make_shared<rt::translate>(back, vector);
        }
    }

    return *this;
}

scene_builder& scene_builder::rotate(const vector3& vector, std::string name)
{
    if (!name.empty())
    {
        auto& found = this->m_objects.get(name);
        if (found)
        {
            found = std::make_shared<rt::rotate>(found, vector);
        }
        else
        {
            // search in groups
            for (auto& group : this->m_groups)
            {
                auto& found2 = group.second->get(name);
                if (found2)
                {
                    found2 = std::make_shared<rt::rotate>(found2, vector);
                    break;
                }
            }
        }
    }
    else
    {
        std::shared_ptr<hittable> back = this->m_objects.back();
        std::string n = back->getName();
        if (n == name)
        {
            this->m_objects.back() = std::make_shared<rt::rotate>(back, vector);
        }
    }

    return *this;
}

scene_builder& scene_builder::scale(const vector3& vector, std::string name)
{
    /*this->m_objects.back() = std::make_shared<rt::scale>(this->m_objects.back(), vector);
    return *this;*/

    if (!name.empty())
    {
        auto& found = this->m_objects.get(name);
        if (found)
        {
            found = std::make_shared<rt::scale>(found, vector);
        }
        else
        {
            // search in groups
            for (auto& group : this->m_groups)
            {
                auto& found2 = group.second->get(name);
                if (found2)
                {
                    found2 = std::make_shared<rt::scale>(found2, vector);
                    break;
                }
            }
        }
    }
    else
    {
        std::shared_ptr<hittable> back = this->m_objects.back();
        std::string n = back->getName();
        if (n == name)
        {
            this->m_objects.back() = std::make_shared<rt::scale>(back, vector);
        }
    }

    return *this;
}

std::shared_ptr<material> scene_builder::fetchMaterial(const std::string& name)
{
    if (!name.empty())
    {
        auto it = this->m_materials.find(name);

        if (it != this->m_materials.end())
        {
            // if key is found
            return it->second;
        }
        else
        {
            // if key is not found
            std::cerr << "[WARN] Material " << name << " not found !" << std::endl;
            return nullptr;
        }
    }

    return nullptr;
}

std::shared_ptr<texture> scene_builder::fetchTexture(const std::string& name)
{
    if (!name.empty())
    {
        auto it = this->m_textures.find(name);

        if (it != this->m_textures.end())
        {
            // if key is found
            return it->second;
        }
        else
        {
            // if key is not found
            std::cerr << "[WARN] Texture " << name << " not found !" << std::endl;
            return nullptr;
        }
    }

    return nullptr;
}