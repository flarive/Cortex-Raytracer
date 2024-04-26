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

  void loadTextures(SceneBuilder &builder, const libconfig::Setting &textures);

  void loadMaterials(SceneBuilder &builder, const libconfig::Setting &setting);

  void loadPrimitives(SceneBuilder &builder, const libconfig::Setting &setting);

  void loadImageConfig(SceneBuilder &builder, const libconfig::Setting &setting);

  void loadCameraConfig(SceneBuilder &builder, const libconfig::Setting &setting);

  point3 getPoint3d(const libconfig::Setting &setting);

  color getRGB(const libconfig::Setting &setting);

  color getRGBColor(const libconfig::Setting &setting);

  Configuration &operator=(const Configuration &) = delete;

  SceneBuilder loadSceneFromFile();

private:
  std::string _path;
  libconfig::Config cfg;
};