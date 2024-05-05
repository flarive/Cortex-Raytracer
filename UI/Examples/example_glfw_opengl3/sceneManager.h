#pragma once

#include "scene.h"

#include <string>
#include <vector>

class sceneManager
{
public:
    void setScenesPath(const std::string path);

    std::vector<scene> listAllScenes();

private:
    std::string m_scenesPath;
};
