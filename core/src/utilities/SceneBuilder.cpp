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
//#include "PlaneXY.hpp"
//#include "PlaneXZ.hpp"
//#include "PlaneYZ.hpp"
#include "../primitives/rotate.h"
#include "../primitives/translate.h"
#include "../primitives/scale.h"

#include "../cameras/target_camera.h"

#include <utility>

#include "ObjectFactory.h"

SceneBuilder::SceneBuilder()
{
  // Default image config:
  this->_config = {225, 400, 100, 50, color(0.0, 0.0, 0.0)};

  // Default camera config:
  this->_camera = {
      16.0 / 9.0, 0.0, {0.0, 0.0, 10.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, 0.0,
      100.0,      70.0};
}

//camera SceneBuilder::getCamera() const
//{
//  return camera(
//      point3(this->_camera.lookFrom.x, this->_camera.lookFrom.y,
//                    this->_camera.lookFrom.z),
//      point3(this->_camera.lookAt.x, this->_camera.lookAt.y,
//                    this->_camera.lookAt.z),
//      vector3(this->_camera.upAxis.x, this->_camera.upAxis.y,
//                      this->_camera.upAxis.z),
//      this->_camera.fov, this->_camera.aspectRatio, this->_camera.aperture,
//      this->_camera.focus, 0.0, this->_camera.openingTime);
//}
//
//hittable_list SceneBuilder::getScene() const {
//  return this->_objects;
//}

imageConfig_t SceneBuilder::getImageConfig() const {
  return this->_config;
}

SceneBuilder& SceneBuilder::imageConfig(const imageConfig_t &config) {
  this->_config = config;
  return *this;
}

SceneBuilder& SceneBuilder::imageBackgroundColor(color c) {
  this->_config.backgroundColor = std::move(c);
  return *this;
}

SceneBuilder& SceneBuilder::imageSize(int width, int height) {
  this->_config.width = width;
  this->_config.height = height;
  return *this;
}

SceneBuilder &SceneBuilder::imageWidth(int width) {
  this->_config.width = width;
  return *this;
}

SceneBuilder& SceneBuilder::imageHeight(int height) {
  this->_config.height = height;
  return *this;
}

SceneBuilder& SceneBuilder::imageWidthWithAspectRatio(double aspectRatio) {
  this->_config.width = int(double(this->_config.height) * aspectRatio);
  return *this;
}

SceneBuilder& SceneBuilder::imageHeightWithAspectRatio(double aspectRatio) {
  this->_config.height = int(double(this->_config.width) / aspectRatio);
  return *this;
}

SceneBuilder& SceneBuilder::imageDepth(int depth) {
  this->_config.depth = depth;
  return *this;
}

SceneBuilder& SceneBuilder::imageSamplesPerPixel(int samplesPerPixel) {
  this->_config.spp = samplesPerPixel;
  return *this;
}
//
//SceneBuilder::cameraConfig(const cameraConfig_t &config) {
//  this->_camera = config;
//  return *this;
//}
//
//SceneBuilder::cameraAspectRatio(double aspectRatio) {
//  this->_camera.aspectRatio = aspectRatio;
//  return *this;
//}
//
//SceneBuilder::cameraOpeningTime(double time) {
//  this->_camera.openingTime = time;
//  return *this;
//}
//
//SceneBuilder::cameraLookFrom(point3 point) {
//  this->_camera.lookFrom = point;
//  return *this;
//}
//
//SceneBuilder::cameraLookAt(point3 lookAt) {
//  this->_camera.lookAt.x = lookAt.x;
//  this->_camera.lookAt.y = lookAt.y;
//  this->_camera.lookAt.z = lookAt.z;
//  return *this;
//}
//
//SceneBuilder& SceneBuilder::cameraUpAxis(point3 vUp) {
//  this->_camera.upAxis.x = vUp.x;
//  this->_camera.upAxis.y = vUp.y;
//  this->_camera.upAxis.z = vUp.z;
//  return *this;
//}
//
//SceneBuilder::cameraAperture(double aperture) {
//  this->_camera.aperture = aperture;
//  return *this;
//}
//
//SceneBuilder& SceneBuilder::cameraFocus(double focus) {
//  this->_camera.focus = focus;
//  return *this;
//}
//
//SceneBuilder& SceneBuilder::cameraFOV(double fov) {
//  this->_camera.fov = fov;
//  return *this;
//}
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
//SceneBuilder& SceneBuilder::addObject(
//    const std::shared_ptr<hittable> &obj) {
//  this->_objects.add(obj);
//  return *this;
//}

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
//cameraConfig_t SceneBuilder::getCameraConfig() const {
//  return this->_camera;
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