#pragma once

#include "../misc/scene.h"

#include "../misc/sceneSettings.h"

#include <string>
#include <vector>
#include <memory>


#pragma comment(lib, "Shlwapi.lib")
#define LIBCONFIGXX_STATIC
#include <libconfig.h++>

class sceneManager
{
public:
    sceneManager();
    void setScenesPath(const std::string path);

    std::vector<scene> listAllScenes();
    std::unique_ptr<sceneSettings> readSceneSettings(std::string filepath);
    void getPostProcessEffectValues(pmap values, short fx_index, float& f1, float& f2);

private:
    std::string m_scenesPath;
    libconfig::Config m_cfg;

    void loadImageConfig(sceneSettings& settings, const libconfig::Setting& setting);
    void loadCameraConfig(sceneSettings& settings, const libconfig::Setting& setting);
    void loadEffectsConfig(sceneSettings& settings, const libconfig::Setting& setting);

    void addPostProcessBloom(const libconfig::Setting& effects, sceneSettings& settings);

    
};
