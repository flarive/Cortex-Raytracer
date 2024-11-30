#include "sceneManager.h"

#include "../utilities/helpers.h"
#include "../misc/sceneSettings.h"

#include "../../../postprocess/effects.h"

#include <iostream>
#include <filesystem>



namespace fs = std::filesystem;

sceneManager::sceneManager()
{
}

void sceneManager::setScenesPath(const std::string path)
{
    m_scenesPath = path;
}


std::vector<scene> sceneManager::listAllScenes()
{
    std::vector<scene> scenes;

    fs::path dir(std::filesystem::current_path());
    fs::path file(this->m_scenesPath);
    fs::path fullexternalProgramPath = dir / file;

    auto fullAbsPath = fs::absolute(fullexternalProgramPath);

    if (fs::exists(fullAbsPath))
    {
        for (const auto& entry : fs::directory_iterator(fullAbsPath))
        {
            std::string name = entry.path().filename().string();
            std::string path = entry.path().generic_string();

            if (name.ends_with(".scene"))
            {
                scene sc = scene(name, path);
                scenes.emplace_back(sc);
            }
        }
    }

    return scenes;
}

std::unique_ptr<sceneSettings> sceneManager::readSceneSettings(std::string filepath)
{
    fs::path dir(std::filesystem::current_path());
    fs::path file(filepath);
    fs::path fullAbsPath = dir / file;

    if (fs::exists(fullAbsPath))
    {
        try
        {
            m_cfg.readFile(fullAbsPath.string());
        }
        catch (const libconfig::ParseException& e)
        {
            std::cerr << "[ERROR] Scene parsing error line " << e.getLine() << " " << e.getError() << std::endl;
            return nullptr;
        }
        catch (const std::exception& e)
        {
            std::cerr << "[ERROR] Scene parsing failed ! " << e.what() << std::endl;
            return nullptr;
        }


        sceneSettings settings;

        const libconfig::Setting& root = m_cfg.getRoot();

        if (root.exists("image"))
        {
            const libconfig::Setting& image = root["image"];
            loadImageConfig(settings, image);
        }

        if (root.exists("camera"))
        {
            const libconfig::Setting& camera = root["camera"];
            loadCameraConfig(settings, camera);
        }

        if (root.exists("effects"))
        {
            const libconfig::Setting& effects = root["effects"];
            loadEffectsConfig(settings, effects);
        }

        return std::make_unique<sceneSettings>(settings);
    }

    return nullptr;
}

void sceneManager::loadImageConfig(sceneSettings& settings, const libconfig::Setting& setting)
{
    if (setting.exists("width"))
        settings.width = setting["width"];
    if (setting.exists("height"))
        settings.height = setting["height"];
    if (setting.exists("maxDepth"))
        settings.depth = setting["maxDepth"];
    if (setting.exists("samplesPerPixel"))
        settings.spp = setting["samplesPerPixel"];
    if (setting.exists("outputFilePath"))
        setting.lookupValue("outputFilePath", settings.outputFilePath);
}

void sceneManager::loadCameraConfig(sceneSettings& settings, const libconfig::Setting& setting)
{
    if (setting.exists("aspectRatio"))
        setting.lookupValue("aspectRatio", settings.aspectRatio);
}


void sceneManager::loadEffectsConfig(sceneSettings& settings, const libconfig::Setting& setting)
{
    addPostProcessBloom(setting, settings);
}


void sceneManager::addPostProcessBloom(const libconfig::Setting& effects, sceneSettings& settings)
{
    if (effects.exists("bloom"))
    {
        for (int i = 0; i < effects["bloom"].getLength(); i++)
        {
            const libconfig::Setting& effect = effects["bloom"][i];
            std::string name;
            float radius = 0.0;
            float threshold = 0.0;
            bool active = true;

            if (effect.exists("name"))
                effect.lookupValue("name", name);
            if (effect.exists("radius"))
                effect.lookupValue("radius", radius);
            if (effect.exists("threshold"))
                effect.lookupValue("threshold", threshold);
            if (effect.exists("active"))
                effect.lookupValue("active", active);

            if (active)
            {
                settings.fx_index = pp_effect::bloom;
                settings.fx_args = std::format("-threshold {} -radius {}", threshold, radius);

                settings.fx_params.clear();
                settings.fx_params.emplace("threshold", threshold);
                settings.fx_params.emplace("radius", radius);
            }
        }
    }
}

void sceneManager::getPostProcessEffectValues(pmap values, short fx_index, float& f1, float&f2)
{
    if (fx_index == pp_effect::bloom)
    {
        if (values.count("threshold"))
        {
            auto v_threshold = values.at("threshold");
            f1 = std::get<float>(v_threshold);
        }

        if (values.count("radius"))
        {
            auto v_radius = values.at("radius");
            f2 = std::get<float>(v_radius);
        }
    }
}

