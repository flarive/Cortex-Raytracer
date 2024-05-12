#pragma once

#include "SceneBuilder.h"
#include "types.h"
#include "../misc/transform.h"
#include "SceneBuilder.h"
#include "types.h"

#pragma comment(lib, "Shlwapi.lib")
#define LIBCONFIGXX_STATIC
#include <libconfig/lib/libconfig.h++>

class Configuration
{
public:
  Configuration(std::string path);
  ~Configuration() = default;

  Configuration(const Configuration &) = delete;

  SceneBuilder loadSceneFromFile();

private:
  std::string _path;
  libconfig::Config cfg;

  void loadPrimitives(SceneBuilder& builder, const libconfig::Setting& setting);
  void loadImageConfig(SceneBuilder& builder, const libconfig::Setting& setting);
  void loadCameraConfig(SceneBuilder& builder, const libconfig::Setting& setting);
  void loadTextures(SceneBuilder& builder, const libconfig::Setting& textures);
  void loadLights(SceneBuilder& builder, const libconfig::Setting& lights);
  void loadMaterials(SceneBuilder& builder, const libconfig::Setting& setting);
  void loadMeshes(SceneBuilder& builder, const libconfig::Setting& setting);

  void applyTransform(const libconfig::Setting& primitive, SceneBuilder& builder);
  point3 getPoint(const libconfig::Setting& setting);
  vector3 getVector(const libconfig::Setting& setting);
  color getColor(const libconfig::Setting& setting);
  uvmapping getUVmapping(const libconfig::Setting& setting);
  rt::transform getTransform(const libconfig::Setting& setting);


  void addImageTexture(const libconfig::Setting& textures, SceneBuilder& builder);
  void addNormalTexture(const libconfig::Setting& textures, SceneBuilder& builder);
  void addNoiseTexture(const libconfig::Setting& textures, SceneBuilder& builder);

};