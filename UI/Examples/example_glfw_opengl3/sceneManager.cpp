#include "sceneManager.h"

#include <iostream>
#include <filesystem>

//#define LIBCONFIGXX_STATIC
//#include <libconfig/lib/libconfig.h++>

namespace fs = std::filesystem;


void sceneManager::setScenesPath(const std::string path)
{
    m_scenesPath = path;
}


std::vector<scene> sceneManager::listAllScenes()
{
    std::vector<scene> scenes;

    fs::path dir(std::filesystem::current_path());
    fs::path file(this->m_scenesPath.c_str());
    fs::path fullexternalProgramPath = dir / file;

    auto fullAbsPath = fs::absolute(fullexternalProgramPath);

    if (fs::exists(fullAbsPath))
    {
        for (const auto& entry : fs::directory_iterator(fullAbsPath))
        {
            scene sc = scene(entry.path().filename().string(), entry.path().generic_string());
            scenes.emplace_back(sc);
        }
    }

    return scenes;
}
