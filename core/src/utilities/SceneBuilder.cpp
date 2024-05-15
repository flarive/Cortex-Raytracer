#include "SceneBuilder.h"

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

#include "ObjectFactory.h"

SceneBuilder::SceneBuilder()
{
  // Default image config
  this->_imageConfig = { 225, 400, 100, 50, color(0.0, 0.0, 0.0) };

  // Default camera config
  this->_cameraConfig = { 16.0 / 9.0, 0.0, {0.0, 0.0, 10.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, 0.0, 100.0, 70.0 };
}

target_camera SceneBuilder::getCamera() const
{
    target_camera cam;
    cam.aspect_ratio = this->_cameraConfig.aspectRatio;
    cam.background_color = color(0, 0, 0);
    cam.image_width = 512;
    cam.lookfrom = point3(this->_cameraConfig.lookFrom.x, this->_cameraConfig.lookFrom.y, this->_cameraConfig.lookFrom.z);
    cam.lookat = point3(this->_cameraConfig.lookAt.x, this->_cameraConfig.lookAt.y, this->_cameraConfig.lookAt.z);
    cam.vup = vector3(this->_cameraConfig.upAxis.x, this->_cameraConfig.upAxis.y, this->_cameraConfig.upAxis.z);
    cam.vfov = this->_cameraConfig.fov;
    cam.max_depth = 50;
    cam.samples_per_pixel = 100;
    cam.defocus_angle = this->_cameraConfig.aperture; // ???
    cam.focus_dist = this->_cameraConfig.focus;

    // this->_camera.openingTime ???????????????

    return cam;
}

hittable_list SceneBuilder::getScene() const
{
  return this->_objects;
}

imageConfig SceneBuilder::getImageConfig() const
{
  return this->_imageConfig;
}

SceneBuilder& SceneBuilder::setImageConfig(const imageConfig &config)
{
  this->_imageConfig = config;
  return *this;
}

SceneBuilder& SceneBuilder::imageBackgroundColor(color c)
{
  this->_imageConfig.backgroundColor = std::move(c);
  return *this;
}

SceneBuilder& SceneBuilder::imageSize(int width, int height)
{
  this->_imageConfig.width = width;
  this->_imageConfig.height = height;
  return *this;
}

SceneBuilder &SceneBuilder::imageWidth(int width)
{
  this->_imageConfig.width = width;
  return *this;
}

SceneBuilder& SceneBuilder::imageHeight(int height)
{
  this->_imageConfig.height = height;
  return *this;
}

SceneBuilder& SceneBuilder::imageWidthWithAspectRatio(double aspectRatio)
{
  this->_imageConfig.width = int(double(this->_imageConfig.height) * aspectRatio);
  return *this;
}

SceneBuilder& SceneBuilder::imageHeightWithAspectRatio(double aspectRatio)
{
  this->_imageConfig.height = int(double(this->_imageConfig.width) / aspectRatio);
  return *this;
}

SceneBuilder& SceneBuilder::imageDepth(int depth)
{
  this->_imageConfig.depth = depth;
  return *this;
}

SceneBuilder& SceneBuilder::imageSamplesPerPixel(int samplesPerPixel)
{
  this->_imageConfig.spp = samplesPerPixel;
  return *this;
}

cameraConfig SceneBuilder::getCameraConfig() const
{
    return this->_cameraConfig;
}

SceneBuilder& SceneBuilder::setCameraConfig(const cameraConfig &config)
{
  this->_cameraConfig = config;
  return *this;
}

SceneBuilder& SceneBuilder::cameraAspectRatio(double aspectRatio)
{
  this->_cameraConfig.aspectRatio = aspectRatio;
  return *this;
}

SceneBuilder& SceneBuilder::cameraOpeningTime(double time)
{
  this->_cameraConfig.openingTime = time;
  return *this;
}

SceneBuilder& SceneBuilder::cameraLookFrom(point3 point)
{
  this->_cameraConfig.lookFrom = point;
  return *this;
}

SceneBuilder& SceneBuilder::cameraLookAt(point3 lookAt)
{
  this->_cameraConfig.lookAt = lookAt;
  return *this;
}

SceneBuilder& SceneBuilder::cameraUpAxis(point3 vUp)
{
  this->_cameraConfig.upAxis = vUp;
  return *this;
}

SceneBuilder& SceneBuilder::cameraAperture(double aperture)
{
  this->_cameraConfig.aperture = aperture;
  return *this;
}

SceneBuilder& SceneBuilder::cameraFocus(double focus)
{
  this->_cameraConfig.focus = focus;
  return *this;
}

SceneBuilder& SceneBuilder::cameraFOV(double fov)
{
  this->_cameraConfig.fov = fov;
  return *this;
}

SceneBuilder& SceneBuilder::addSolidColorTexture(const std::string& textureName, color rgb)
{
  this->_textures[textureName] = std::make_shared<solid_color_texture>(rgb);
  return *this;
}

SceneBuilder& SceneBuilder::addGradientColorTexture(const std::string& textureName, color color1, color color2, bool aligned_v, bool hsv)
{
	this->_textures[textureName] = std::make_shared<gradient_texture>(color1, color2, aligned_v, hsv);
	return *this;
}

SceneBuilder& SceneBuilder::addCheckerTexture(const std::string& textureName, double scale, color oddColor, color evenColor)
{
	this->_textures[textureName] = std::make_shared<checker_texture>(scale, oddColor, evenColor);
	return *this;
}

SceneBuilder& SceneBuilder::addCheckerTexture(const std::string& textureName, double scale, const std::string &oddTextureName, const std::string &evenTextureName)
{
  this->_textures[textureName] = std::make_shared<checker_texture>(scale, this->_textures[oddTextureName], this->_textures[evenTextureName]);
  return *this;
}

SceneBuilder& SceneBuilder::addImageTexture(const std::string& textureName, const std::string &filepath)
{
  this->_textures[textureName] = std::make_shared<image_texture>(filepath);
  return *this;
}

SceneBuilder& SceneBuilder::addNormalTexture(const std::string& textureName, const std::string& filepath)
{
    auto normal_tex = std::make_shared<image_texture>(filepath);
    this->_textures[textureName] = std::make_shared<normal_texture>(normal_tex);
    return *this;
}

SceneBuilder& SceneBuilder::addNoiseTexture(const std::string& textureName, double scale)
{
  this->_textures[textureName] = std::make_shared<perlin_noise_texture>(scale);
  return *this;
}

SceneBuilder& SceneBuilder::addMarbleTexture(const std::string& textureName, double scale)
{
	this->_textures[textureName] = std::make_shared<marble_texture>(scale);
	return *this;
}

SceneBuilder& SceneBuilder::addBumpTexture(const std::string& textureName, const std::string& diffuseTextureName, const std::string& bumpTextureName, double scale)
{
    this->_textures[textureName] = std::make_shared<bump_texture>(this->_textures[diffuseTextureName], this->_textures[bumpTextureName], scale);
    return *this;
}

SceneBuilder& SceneBuilder::addGlassMaterial(const std::string &materialName, double refraction)
{
  this->_materials[materialName] = std::make_shared<dielectric>(refraction);
  return *this;
}

SceneBuilder& SceneBuilder::addLambertianMaterial(const std::string& materialName, const color& rgb)
{
  this->_materials[materialName] = std::make_shared<lambertian>(rgb);
  return *this;
}

SceneBuilder& SceneBuilder::addLambertianMaterial(const std::string& materialName, const std::string& textureName)
{
  this->_materials[materialName] = std::make_shared<lambertian>(this->_textures[textureName]);
  return *this;
}

SceneBuilder& SceneBuilder::addPhongMaterial(const std::string& materialName, const color& rgb, double ambiant, double diffuse, double specular, double shininess, double transparency, double refraction_index)
{
    this->_materials[materialName] = std::make_shared<phong>(rgb, ambiant, diffuse, specular, shininess, transparency, refraction_index);
    return *this;
}

SceneBuilder& SceneBuilder::addPhongMaterial(const std::string& materialName, const std::string& albedoTextureName, const std::string& normalTextureName, double ambiant, double diffuse, double specular, double shininess, double transparency, double refraction_index)
{
    this->_materials[materialName] = std::make_shared<phong>(
        this->_textures[albedoTextureName],
        !normalTextureName.empty() ? this->_textures[normalTextureName] : nullptr,
        ambiant, diffuse, specular, shininess, transparency, refraction_index);
    return *this;
}

SceneBuilder& SceneBuilder::addPhong2Material(const std::string& materialName, const std::string& diffuseTextureName, const std::string& specularTextureName, const std::string& normalTextureName, const color& ambient, double shininess)
{
    this->_materials[materialName] = std::make_shared<phong2>(
        !diffuseTextureName.empty() ? this->_textures[diffuseTextureName] : nullptr,
        !specularTextureName.empty() ? this->_textures[specularTextureName] : nullptr,
        !normalTextureName.empty() ? this->_textures[normalTextureName] : nullptr,
        ambient, shininess);
    return *this;
}

SceneBuilder& SceneBuilder::addOrenNayarMaterial(const std::string& materialName, const color& rgb, double albedo_temp, double roughness)
{
	this->_materials[materialName] = std::make_shared<oren_nayar>(rgb, albedo_temp, roughness);
	return *this;
}

SceneBuilder& SceneBuilder::addOrenNayarMaterial(const std::string& materialName, const std::string& textureName, double albedo_temp, double roughness)
{
	this->_materials[materialName] = std::make_shared<oren_nayar>(this->_textures[textureName], albedo_temp, roughness);
	return *this;
}

SceneBuilder& SceneBuilder::addIsotropicMaterial(const std::string& materialName, const color& rgb)
{
    this->_materials[materialName] = std::make_shared<isotropic>(rgb);
    return *this;
}

SceneBuilder& SceneBuilder::addIsotropicMaterial(const std::string& materialName, const std::string& textureName)
{
    this->_materials[materialName] = std::make_shared<isotropic>(this->_textures[textureName]);
    return *this;
}

//SceneBuilder& SceneBuilder::addAnisotropicMaterial(const std::string& materialName, const color& rgb, double roughness)
//{
//    this->_materials[materialName] = std::make_shared<anisotropic>(rgb, roughness);
//    return *this;
//}

SceneBuilder& SceneBuilder::addAnisotropicMaterial(const std::string& materialName, double nu, double nv, const std::string& diffuseTextureName, const std::string& specularTextureName, const std::string& exponentTextureName)
{
    this->_materials[materialName] = std::make_shared<anisotropic>(nu, nv, this->_textures[diffuseTextureName], this->_textures[specularTextureName], this->_textures[exponentTextureName]);
    return *this;
}

SceneBuilder& SceneBuilder::addMetalMaterial(const std::string &materialName, color rgb, double fuzz)
{
  this->_materials[materialName] = std::make_shared<metal>(rgb, fuzz);
  return *this;
}

SceneBuilder& SceneBuilder::addDirectionalLight(const point3& pos, const vector3& u, const vector3& v, double intensity, color rgb, bool invisible, std::string name)
{
    this->_objects.add(
        ObjectFactory::createDirectionalLight(
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

SceneBuilder& SceneBuilder::addOmniDirectionalLight(const point3& pos, double radius, double intensity, color rgb, bool invisible, std::string name)
{
    this->_objects.add(
        ObjectFactory::createOmniDirectionalLight(
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

//SceneBuilder& RayTracer::SceneBuilder::addDirectionalLightMaterial(const std::string &materialName, const std::string &textureName) {
//  this->_materials[materialName] =
//      std::make_shared<diffuse_light>(this->_textures[textureName]);
//  return *this;
//}
//
//SceneBuilder::setAmbianceLight(color rgb) {
//  this->_config.backgroundColor[0] = rgb.r();
//  this->_config.backgroundColor[1] = rgb.g();
//  this->_config.backgroundColor[2] = rgb.b();
//  return *this;
//}
//
SceneBuilder& SceneBuilder::addObject(const std::shared_ptr<hittable> &obj)
{
  this->_objects.add(obj);
  return *this;
}

SceneBuilder& SceneBuilder::addSphere(std::string name, point3 pos, double radius, const std::string& material, const uvmapping& uv)
{
	this->_objects.add(
		ObjectFactory::createSphere(
			name,
            pos,
			radius,
			this->_materials[material],
            uv
		)
	);
	return *this;
}

SceneBuilder& SceneBuilder::addPlane(std::string name, point3 p0, point3 p1, const std::string &material, const uvmapping& uv)
{
    this->_objects.add(
        ObjectFactory::createPlane(
            name,
            p0,
            p1,
            this->_materials[material],
            uv
        )
    );
  return *this;
}

SceneBuilder& SceneBuilder::addBox(std::string name, point3 p0, point3 p1, const std::string& material, const uvmapping& uv)
{
    this->_objects.add(
        ObjectFactory::createBox(
            name,    
            p0,
            p1,
            this->_materials[material],
            uv
        )
    );
  return *this;
}

SceneBuilder& SceneBuilder::addCylinder(std::string name, point3 pos, double radius, double height, const std::string &material, const uvmapping& uv)
{
    this->_objects.add(
        ObjectFactory::createCylinder(
            name,
            pos,
            radius,
            height,
            this->_materials[material],
            uv
        )
    );
  return *this;
}

SceneBuilder& SceneBuilder::addDisk(std::string name, point3 pos, double radius, double height, const std::string& material, const uvmapping& uv)
{
    this->_objects.add(
        ObjectFactory::createDisk(
            name,
            pos,
            radius,
            height,
            this->_materials[material],
            uv
        )
    );
    return *this;
}

SceneBuilder& SceneBuilder::addTorus(std::string name, point3 pos, double major_radius, double minor_radius, const std::string& material, const uvmapping& uv)
{
    this->_objects.add(
        ObjectFactory::createTorus(
            name,
            pos,
            major_radius,
            minor_radius,
            this->_materials[material],
            uv
        )
    );
    return *this;
}

SceneBuilder& SceneBuilder::addCone(std::string name, point3 pos, double radius, double height, const std::string &material, const uvmapping& uv)
{
    this->_objects.add(
        ObjectFactory::createCone(
            name,
            pos,
                height,
                radius,
                this->_materials[material],
                uv
        )
    );
  return *this;
}

SceneBuilder& SceneBuilder::addMesh(std::string name, point3 pos, const std::string& filepath, const std::string& material, bool use_mtl, bool use_smoothing)
{
	this->_objects.add(
		ObjectFactory::createMesh(
			name,
			pos,
            filepath,
            !material.empty() ? this->_materials[material] : nullptr,
			use_mtl,
            use_smoothing
		)
	);
	return *this;
}

SceneBuilder& SceneBuilder::translate(const vector3& vector)
{
  this->_objects.back() = std::make_shared<rt::translate>(this->_objects.back(), vector);
  return *this;
}

SceneBuilder& SceneBuilder::rotate(const vector3& vector)
{
  this->_objects.back() = std::make_shared<rt::rotate>(this->_objects.back(), vector);
  return *this;
}

SceneBuilder& SceneBuilder::scale(const vector3& vector)
{
    this->_objects.back() = std::make_shared<rt::scale>(this->_objects.back(), vector);
    return *this;
}