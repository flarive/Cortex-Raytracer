#include "scene_builder.h"

#include "../primitives/box.h"
#include "../primitives/cone.h"
#include "../primitives/cylinder.h"
#include "../primitives/sphere.h"
#include "../primitives/aarect.h"

#include "../materials/dielectric.h"
#include "../materials/lambertian.h"
#include "../materials/phong.h"
#include "../materials/phong2.h"
#include "../materials/oren_nayar.h"
#include "../materials/diffuse_light.h"
#include "../materials/metal.h"
#include "../materials/isotropic.h"
#include "../materials/anisotropic.h"

#include "../textures/checker_texture.h"
#include "../textures/perlin_noise_texture.h"
#include "../textures/solid_color_texture.h"
#include "../textures/image_texture.h"
#include "../textures/normal_texture.h"
#include "../textures/gradient_texture.h"
#include "../textures/marble_texture.h"
#include "../textures/bump_texture.h"

#include "../lights/quad_light.h"
#include "../lights/sphere_light.h"

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
  this->m_cameraConfig = { 16.0 / 9.0, 0.0, {0.0, 0.0, 10.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, 0.0, 100.0, 70.0 };
}

target_camera scene_builder::getCamera() const
{
    target_camera cam;
    cam.aspect_ratio = this->m_cameraConfig.aspectRatio;
    cam.background_color = color(0, 0, 0);
    cam.image_width = 512;
    cam.lookfrom = point3(this->m_cameraConfig.lookFrom.x, this->m_cameraConfig.lookFrom.y, this->m_cameraConfig.lookFrom.z);
    cam.lookat = point3(this->m_cameraConfig.lookAt.x, this->m_cameraConfig.lookAt.y, this->m_cameraConfig.lookAt.z);
    cam.vup = vector3(this->m_cameraConfig.upAxis.x, this->m_cameraConfig.upAxis.y, this->m_cameraConfig.upAxis.z);
    cam.vfov = this->m_cameraConfig.fov;
    cam.max_depth = 50;
    cam.samples_per_pixel = 100;
    cam.defocus_angle = this->m_cameraConfig.aperture; // ???
    cam.focus_dist = this->m_cameraConfig.focus;

    // this->_camera.openingTime ???????????????

    return cam;
}

hittable_list scene_builder::getScene() const
{
  return this->m_objects;
}

imageConfig scene_builder::getImageConfig() const
{
  return this->m_imageConfig;
}

scene_builder& scene_builder::setImageConfig(const imageConfig &config)
{
  this->m_imageConfig = config;
  return *this;
}

scene_builder& scene_builder::imageBackgroundColor(color c)
{
  this->m_imageConfig.backgroundColor = std::move(c);
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

cameraConfig scene_builder::getCameraConfig() const
{
    return this->m_cameraConfig;
}

scene_builder& scene_builder::setCameraConfig(const cameraConfig &config)
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
  this->m_cameraConfig.focus = focus;
  return *this;
}

scene_builder& scene_builder::cameraFOV(double fov)
{
  this->m_cameraConfig.fov = fov;
  return *this;
}

scene_builder& scene_builder::addSolidColorTexture(const std::string& textureName, color rgb)
{
  this->m_textures[textureName] = std::make_shared<solid_color_texture>(rgb);
  return *this;
}

scene_builder& scene_builder::addGradientColorTexture(const std::string& textureName, color color1, color color2, bool aligned_v, bool hsv)
{
	this->m_textures[textureName] = std::make_shared<gradient_texture>(color1, color2, aligned_v, hsv);
	return *this;
}

scene_builder& scene_builder::addCheckerTexture(const std::string& textureName, double scale, color oddColor, color evenColor)
{
	this->m_textures[textureName] = std::make_shared<checker_texture>(scale, oddColor, evenColor);
	return *this;
}

scene_builder& scene_builder::addCheckerTexture(const std::string& textureName, double scale, const std::string &oddTextureName, const std::string &evenTextureName)
{
  this->m_textures[textureName] = std::make_shared<checker_texture>(scale, this->m_textures[oddTextureName], this->m_textures[evenTextureName]);
  return *this;
}

scene_builder& scene_builder::addImageTexture(const std::string& textureName, const std::string &filepath)
{
  this->m_textures[textureName] = std::make_shared<image_texture>(filepath);
  return *this;
}

scene_builder& scene_builder::addNormalTexture(const std::string& textureName, const std::string& filepath)
{
    auto normal_tex = std::make_shared<image_texture>(filepath);
    this->m_textures[textureName] = std::make_shared<normal_texture>(normal_tex);
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

scene_builder& scene_builder::addBumpTexture(const std::string& textureName, const std::string& diffuseTextureName, const std::string& bumpTextureName, double scale)
{
    this->m_textures[textureName] = std::make_shared<bump_texture>(this->m_textures[diffuseTextureName], this->m_textures[bumpTextureName], scale);
    return *this;
}

scene_builder& scene_builder::addGlassMaterial(const std::string &materialName, double refraction)
{
  this->m_materials[materialName] = std::make_shared<dielectric>(refraction);
  return *this;
}

scene_builder& scene_builder::addLambertianMaterial(const std::string& materialName, const color& rgb)
{
  this->m_materials[materialName] = std::make_shared<lambertian>(rgb);
  return *this;
}

scene_builder& scene_builder::addLambertianMaterial(const std::string& materialName, const std::string& textureName)
{
  this->m_materials[materialName] = std::make_shared<lambertian>(this->m_textures[textureName]);
  return *this;
}

scene_builder& scene_builder::addPhongMaterial(const std::string& materialName, const color& rgb, double ambiant, double diffuse, double specular, double shininess, double transparency, double refraction_index)
{
    this->m_materials[materialName] = std::make_shared<phong>(rgb, ambiant, diffuse, specular, shininess, transparency, refraction_index);
    return *this;
}

scene_builder& scene_builder::addPhongMaterial(const std::string& materialName, const std::string& albedoTextureName, const std::string& normalTextureName, double ambiant, double diffuse, double specular, double shininess, double transparency, double refraction_index)
{
    this->m_materials[materialName] = std::make_shared<phong>(
        this->m_textures[albedoTextureName],
        !normalTextureName.empty() ? this->m_textures[normalTextureName] : nullptr,
        ambiant, diffuse, specular, shininess, transparency, refraction_index);
    return *this;
}

scene_builder& scene_builder::addPhong2Material(const std::string& materialName, const std::string& diffuseTextureName, const std::string& specularTextureName, const std::string& normalTextureName, const color& ambient, double shininess)
{
    this->m_materials[materialName] = std::make_shared<phong2>(
        !diffuseTextureName.empty() ? this->m_textures[diffuseTextureName] : nullptr,
        !specularTextureName.empty() ? this->m_textures[specularTextureName] : nullptr,
        !normalTextureName.empty() ? this->m_textures[normalTextureName] : nullptr,
        ambient, shininess);
    return *this;
}

scene_builder& scene_builder::addOrenNayarMaterial(const std::string& materialName, const color& rgb, double albedo_temp, double roughness)
{
	this->m_materials[materialName] = std::make_shared<oren_nayar>(rgb, albedo_temp, roughness);
	return *this;
}

scene_builder& scene_builder::addOrenNayarMaterial(const std::string& materialName, const std::string& textureName, double albedo_temp, double roughness)
{
	this->m_materials[materialName] = std::make_shared<oren_nayar>(this->m_textures[textureName], albedo_temp, roughness);
	return *this;
}

scene_builder& scene_builder::addIsotropicMaterial(const std::string& materialName, const color& rgb)
{
    this->m_materials[materialName] = std::make_shared<isotropic>(rgb);
    return *this;
}

scene_builder& scene_builder::addIsotropicMaterial(const std::string& materialName, const std::string& textureName)
{
    this->m_materials[materialName] = std::make_shared<isotropic>(this->m_textures[textureName]);
    return *this;
}

//scene_builder& scene_builder::addAnisotropicMaterial(const std::string& materialName, const color& rgb, double roughness)
//{
//    this->_materials[materialName] = std::make_shared<anisotropic>(rgb, roughness);
//    return *this;
//}

scene_builder& scene_builder::addAnisotropicMaterial(const std::string& materialName, double nu, double nv, const std::string& diffuseTextureName, const std::string& specularTextureName, const std::string& exponentTextureName)
{
    this->m_materials[materialName] = std::make_shared<anisotropic>(nu, nv, this->m_textures[diffuseTextureName], this->m_textures[specularTextureName], this->m_textures[exponentTextureName]);
    return *this;
}

scene_builder& scene_builder::addMetalMaterial(const std::string &materialName, color rgb, double fuzz)
{
  this->m_materials[materialName] = std::make_shared<metal>(rgb, fuzz);
  return *this;
}

scene_builder& scene_builder::addDirectionalLight(const point3& pos, const vector3& u, const vector3& v, double intensity, color rgb, bool invisible, std::string name)
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

scene_builder& scene_builder::addOmniDirectionalLight(const point3& pos, double radius, double intensity, color rgb, bool invisible, std::string name)
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

//scene_builder& RayTracer::scene_builder::addDirectionalLightMaterial(const std::string &materialName, const std::string &textureName) {
//  this->_materials[materialName] =
//      std::make_shared<diffuse_light>(this->_textures[textureName]);
//  return *this;
//}
//
//scene_builder::setAmbianceLight(color rgb) {
//  this->_config.backgroundColor[0] = rgb.r();
//  this->_config.backgroundColor[1] = rgb.g();
//  this->_config.backgroundColor[2] = rgb.b();
//  return *this;
//}
//
scene_builder& scene_builder::addObject(const std::shared_ptr<hittable> &obj)
{
  this->m_objects.add(obj);
  return *this;
}

scene_builder& scene_builder::addSphere(std::string name, point3 pos, double radius, const std::string& material, const uvmapping& uv)
{
	this->m_objects.add(
		scene_factory::createSphere(
			name,
            pos,
			radius,
			this->m_materials[material],
            uv
		)
	);
	return *this;
}

scene_builder& scene_builder::addPlane(std::string name, point3 p0, point3 p1, const std::string &material, const uvmapping& uv)
{
    this->m_objects.add(
        scene_factory::createPlane(
            name,
            p0,
            p1,
            this->m_materials[material],
            uv
        )
    );
  return *this;
}

scene_builder& scene_builder::addQuad(std::string name, point3 position, vector3 u, vector3 v, const std::string& material, const uvmapping& uv)
{
    this->m_objects.add(
        scene_factory::createQuad(
            name,
            position,
            u,
            v,
            this->m_materials[material],
            uv
        )
    );
    return *this;
}

scene_builder& scene_builder::addBox(std::string name, point3 p0, point3 p1, const std::string& material, const uvmapping& uv)
{
    this->m_objects.add(
        scene_factory::createBox(
            name,    
            p0,
            p1,
            this->m_materials[material],
            uv
        )
    );
  return *this;
}

scene_builder& scene_builder::addCylinder(std::string name, point3 pos, double radius, double height, const std::string &material, const uvmapping& uv)
{
    this->m_objects.add(
        scene_factory::createCylinder(
            name,
            pos,
            radius,
            height,
            this->m_materials[material],
            uv
        )
    );
  return *this;
}

scene_builder& scene_builder::addDisk(std::string name, point3 pos, double radius, double height, const std::string& material, const uvmapping& uv)
{
    this->m_objects.add(
        scene_factory::createDisk(
            name,
            pos,
            radius,
            height,
            this->m_materials[material],
            uv
        )
    );
    return *this;
}

scene_builder& scene_builder::addTorus(std::string name, point3 pos, double major_radius, double minor_radius, const std::string& material, const uvmapping& uv)
{
    this->m_objects.add(
        scene_factory::createTorus(
            name,
            pos,
            major_radius,
            minor_radius,
            this->m_materials[material],
            uv
        )
    );
    return *this;
}

scene_builder& scene_builder::addCone(std::string name, point3 pos, double radius, double height, const std::string &material, const uvmapping& uv)
{
    this->m_objects.add(
        scene_factory::createCone(
            name,
            pos,
                height,
                radius,
                this->m_materials[material],
                uv
        )
    );
  return *this;
}

scene_builder& scene_builder::addVolume(std::string name, std::string boundaryObjectName, double density, const std::string& textureName)
{
    auto boundaryObject = this->m_objects.get(boundaryObjectName);
    if (boundaryObject)
    {
        this->m_objects.add(
            scene_factory::createVolume(
                name,
                boundaryObject,
                density,
                this->m_textures[textureName]
            )
        );

        this->m_objects.remove(boundaryObject);
    }

    return *this;
}

scene_builder& scene_builder::addVolume(std::string name, std::string boundaryObjectName, double density, const color& rgb)
{
    auto boundaryObject = this->m_objects.get(boundaryObjectName);
    if (boundaryObject)
    {
        this->m_objects.add(
            scene_factory::createVolume(
                name,
                boundaryObject,
                density,
                rgb
            )
        );

        this->m_objects.remove(boundaryObject);
    }

    return *this;
}

scene_builder& scene_builder::addMesh(std::string name, point3 pos, const std::string& filepath, const std::string& material, bool use_mtl, bool use_smoothing)
{
	this->m_objects.add(
		scene_factory::createMesh(
			name,
			pos,
            filepath,
            !material.empty() ? this->m_materials[material] : nullptr,
			use_mtl,
            use_smoothing
		)
	);
	return *this;
}

scene_builder& scene_builder::translate(const vector3& vector)
{
  this->m_objects.back() = std::make_shared<rt::translate>(this->m_objects.back(), vector);
  return *this;
}

scene_builder& scene_builder::rotate(const vector3& vector)
{
  this->m_objects.back() = std::make_shared<rt::rotate>(this->m_objects.back(), vector);
  return *this;
}

scene_builder& scene_builder::scale(const vector3& vector)
{
    this->m_objects.back() = std::make_shared<rt::scale>(this->m_objects.back(), vector);
    return *this;
}