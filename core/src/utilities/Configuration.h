#pragma once

#include "../primitives/hittable.h"
#include "SceneBuilder.h"
#include "types.h"
#include <fstream>
#include <iostream>

#include "SceneBuilder.h"
#include "types.h"

#include <libconfig/lib/libconfig.h++>

class Configuration
{
public:
  Configuration(std::string path);
  ~Configuration() = default;

  Configuration(const Configuration &) = delete;

  void loadPrimitives(SceneBuilder &builder, const libconfig::Setting &setting);

  void loadImageConfig(SceneBuilder &builder, const libconfig::Setting &setting);

  void loadCameraConfig(SceneBuilder &builder, const libconfig::Setting &setting);

  void loadTextures(SceneBuilder& builder, const libconfig::Setting& textures);

  void loadLights(SceneBuilder& builder, const libconfig::Setting& lights);

  void loadMaterials(SceneBuilder& builder, const libconfig::Setting& setting);

  point3 getPoint(const libconfig::Setting &setting);

  vector3 getVector(const libconfig::Setting& setting);

  color getColor(const libconfig::Setting &setting);

  uvmapping getUVmapping(const libconfig::Setting& setting);

  Configuration &operator=(const Configuration &) = delete;

  SceneBuilder loadSceneFromFile();

private:
  std::string _path;
  libconfig::Config cfg;

};