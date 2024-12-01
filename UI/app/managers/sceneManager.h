#pragma once

#include "../misc/scene.h"

#include "../misc/sceneSettings.h"

#include <string>
#include <vector>
#include <memory>


#pragma comment(lib, "shlwapi.lib")
#define LIBCONFIGXX_STATIC
#include <libconfig.h++>

class sceneManager
{
public:
    sceneManager();
    void setScenesPath(const std::string path);

    std::vector<scene> listAllScenes();
    std::unique_ptr<sceneSettings> readSceneSettings(std::string filepath);
    void getPostProcessBloomEffectValues(pmap values, short fx_index, float& threshold, float& radius, float& intensity, float& maxbloom);
    void getPostProcessContrastSaturationBrightnessEffectValues(pmap values, short fx_index, float& contrast, float& saturation, float& brightness);

private:
    std::string m_scenesPath;
    libconfig::Config m_cfg;

    void loadImageConfig(sceneSettings& settings, const libconfig::Setting& setting);
    void loadCameraConfig(sceneSettings& settings, const libconfig::Setting& setting);
    void loadEffectsConfig(sceneSettings& settings, const libconfig::Setting& setting);

    void addPostProcessBloom(const libconfig::Setting& effects, sceneSettings& settings);
    void addPostProcessFloydSteinberg(const libconfig::Setting& effects, sceneSettings& settings);
    void addPostProcessContrastSaturationBrightness(const libconfig::Setting& effects, sceneSettings& settings);
};
