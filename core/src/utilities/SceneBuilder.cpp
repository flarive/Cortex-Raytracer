#include "SceneBuilder.h"

#include "../primitives/box.h"
#include "../primitives/cone.h"
#include "../primitives/cylinder.h"
#include "../primitives/sphere.h"
#include "../primitives/aarect.h"

#include "../materials/dielectric.h"
#include "../materials/lambertian.h"
#include "../materials/diffuse_light.h"
#include "../materials/metal.h"
#include "../textures/checker_texture.h"
#include "../textures/perlin_noise_texture.h"
#include "../textures/solid_color_texture.h"
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

SceneBuilder& SceneBuilder::setImageConfig(const imageConfig &config) {
  this->_imageConfig = config;
  return *this;
}

SceneBuilder& SceneBuilder::imageBackgroundColor(color c) {
  this->_imageConfig.backgroundColor = std::move(c);
  return *this;
}

SceneBuilder& SceneBuilder::imageSize(int width, int height) {
  this->_imageConfig.width = width;
  this->_imageConfig.height = height;
  return *this;
}

SceneBuilder &SceneBuilder::imageWidth(int width) {
  this->_imageConfig.width = width;
  return *this;
}

SceneBuilder& SceneBuilder::imageHeight(int height) {
  this->_imageConfig.height = height;
  return *this;
}

SceneBuilder& SceneBuilder::imageWidthWithAspectRatio(double aspectRatio) {
  this->_imageConfig.width = int(double(this->_imageConfig.height) * aspectRatio);
  return *this;
}

SceneBuilder& SceneBuilder::imageHeightWithAspectRatio(double aspectRatio) {
  this->_imageConfig.height = int(double(this->_imageConfig.width) / aspectRatio);
  return *this;
}

SceneBuilder& SceneBuilder::imageDepth(int depth) {
  this->_imageConfig.depth = depth;
  return *this;
}

SceneBuilder& SceneBuilder::imageSamplesPerPixel(int samplesPerPixel) {
  this->_imageConfig.spp = samplesPerPixel;
  return *this;
}




cameraConfig SceneBuilder::getCameraConfig() const
{
    return this->_cameraConfig;
}

SceneBuilder& SceneBuilder::setCameraConfig(const cameraConfig &config) {
  this->_cameraConfig = config;
  return *this;
}

SceneBuilder& SceneBuilder::cameraAspectRatio(double aspectRatio) {
  this->_cameraConfig.aspectRatio = aspectRatio;
  return *this;
}

SceneBuilder& SceneBuilder::cameraOpeningTime(double time) {
  this->_cameraConfig.openingTime = time;
  return *this;
}

SceneBuilder& SceneBuilder::cameraLookFrom(point3 point) {
  this->_cameraConfig.lookFrom = point;
  return *this;
}

SceneBuilder& SceneBuilder::cameraLookAt(point3 lookAt) {
  this->_cameraConfig.lookAt = lookAt;
  return *this;
}

SceneBuilder& SceneBuilder::cameraUpAxis(point3 vUp) {
  this->_cameraConfig.upAxis = vUp;
  return *this;
}

SceneBuilder& SceneBuilder::cameraAperture(double aperture) {
  this->_cameraConfig.aperture = aperture;
  return *this;
}

SceneBuilder& SceneBuilder::cameraFocus(double focus) {
  this->_cameraConfig.focus = focus;
  return *this;
}

SceneBuilder& SceneBuilder::cameraFOV(double fov) {
  this->_cameraConfig.fov = fov;
  return *this;
}
//
//SceneBuilder& SceneBuilder::addTexture(
//    const std::string &setTextureName,
//    const std::shared_ptr<texture> &texture) {
//  this->_textures[setTextureName] = texture;
//  return *this;
//}
//
//SceneBuilder::addSolidColorTexture(const std::string &setTextureName, color rgb) {
//  this->_textures[setTextureName] =
//      std::make_shared<solid_color_texture>(rgb.r, rgb.g, rgb.b);
//  return *this;
//}
//
//SceneBuilder& SceneBuilder::addChessBoardTexture(
//    const std::string &setTextureName, color rgbLight, color rgbDark) {
//  this->_textures[setTextureName] = std::make_shared<checker_texture>(
//      vector3(rgbLight.r, rgbLight.g, rgbLight.b),
//      vector3(rgbDark.r, rgbDark.g, rgbDark.b));
//  return *this;
//}
//
//SceneBuilder::addChessBoardTexture(const std::string &setTextureName,
//                                              const std::string &textureLight,
//                                              const std::string &textureDark) {
//  this->_textures[setTextureName] = std::make_shared<checker_texture>(
//      this->_textures[textureLight], this->_textures[textureDark]);
//  return *this;
//}
//
//SceneBuilder::addNoiseTexture(const std::string &setTextureName, double scale) {
//  this->_textures[setTextureName] =
//      std::make_shared<perlin_noise_texture>(scale);
//  return *this;
//}
//
//SceneBuilder& SceneBuilder::addMaterial(
//    const std::string &materialName,
//    const std::shared_ptr<material> &material) {
//  this->_materials[materialName] = material;
//  return *this;
//}
//
//SceneBuilder::addGlassMaterial(const std::string &materialName, double refraction) {
//  this->_materials[materialName] =
//      std::make_shared<dielectric>(refraction);
//  return *this;
//}
//
//SceneBuilder::addSolidMaterial(const std::string &materialName, color rgb) {
//  this->_materials[materialName] = std::make_shared<lambertian>(
//      vector3(rgb.r, rgb.g, rgb.b));
//  return *this;
//}
//
//SceneBuilder::addSolidMaterial(const std::string &materialName, const std::string &textureName) {
//  this->_materials[materialName] =
//      std::make_shared<lambertian>(this->_textures[textureName]);
//  return *this;
//}
//
//SceneBuilder::addMetalMaterial(const std::string &materialName, color rgb, double fuzz) {
//  this->_materials[materialName] = std::make_shared<metal>(
//      vector3(rgb.r, rgb.g, rgb.b), fuzz);
//  return *this;
//}
//
//SceneBuilder& SceneBuilder::addDirectionalLightMaterial(
//    const std::string &materialName, color rgb) {
//  this->_materials[materialName] = std::make_shared<diffuse_light>(
//      vector3(rgb.r, rgb.g, rgb.b));
//  return *this;
//}
//
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

SceneBuilder& SceneBuilder::addSphere(point3 pos, double radius, const std::string& material) {
	this->_objects.add(
		ObjectFactory::createSphere(
			point3(pos.x, pos.y, pos.z),
			radius,
			this->_materials[material]
		)
	);
	return *this;
}

//SceneBuilder::addPlane(point3 p0, point3 p1, const std::string &material) {
//    this->_objects.add(
//            ObjectFactory::createPlane(
//                    point3(p0.x, p0.y, p0.z),
//                    point3(p1.x, p1.y, p1.z),
//                    this->_materials[material]
//            )
//    );
//  return *this;
//}
//
//SceneBuilder::addBox(point3 p0, point3 p1, const std::string &material) {
//    this->_objects.add(
//            ObjectFactory::createBox(
//                    point3(p0.x, p0.y, p0.z),
//                    point3(p1.x, p1.y, p1.z),
//                    this->_materials[material]
//            )
//    );
//  return *this;
//}
//
//SceneBuilder::addCylinder(point3 pos, double radius,
//                                     double height,
//                                     const std::string &material) {
//    this->_objects.add(
//            ObjectFactory::createCylinder(
//                    point3(pos.x, pos.y, pos.z),
//                    radius,
//                    height,
//                    this->_materials[material]
//            )
//    );
//  return *this;
//}
//
//SceneBuilder::addCone(point3 pos, double radius, double height, const std::string &material) {
//    this->_objects.add(
//            ObjectFactory::createCone(
//                    point3(pos.x, pos.y, pos.z),
//                    height,
//                    radius,
//                    this->_materials[material]
//            )
//    );
//  return *this;
//}
//
//SceneBuilder::translate(point3 trs) {
//  this->_objects.back() = std::make_shared<raytracer::translate>(
//      this->_objects.back(), vector3(trs.x, trs.y, trs.z));
//  return *this;
//}
//
//SceneBuilder& SceneBuilder::rotateY(double angle) {
//  this->_objects.back() =
//      std::make_shared<raytracer::rotate>(this->_objects.back(), angle);
//  return *this;
//}


//
//std::map<std::string, std::shared_ptr<texture>>
//SceneBuilder::getTextures() const {
//  return this->_textures;
//}
//
//std::map<std::string, std::shared_ptr<material>>
//SceneBuilder::getMaterials() const {
//  return this->_materials;
//}