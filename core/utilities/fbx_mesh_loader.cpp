#include "fbx_mesh_loader.h"

#include <ofbx.h>

#include <filesystem>

fbx_mesh_loader::fbx_mesh_loader()
{
}

bool fbx_mesh_loader::load_model_from_file(const std::string& filepath, fbx_mesh_data& data)
{
    std::filesystem::path dir(std::filesystem::current_path());
    std::filesystem::path file(filepath);
    std::filesystem::path fullPath = dir / file;

    std::cout << "[INFO] Loading FBX file " << fullPath.filename() << std::endl;

    auto fullAbsPath = std::filesystem::absolute(fullPath);
    if (!std::filesystem::exists(fullAbsPath)) {
        std::cerr << "[ERROR] FBX file not found: " << fullAbsPath << std::endl;
        return false;
    }

    std::string inputFile = fullAbsPath.generic_string();


	static char s_TimeString[256];
	FILE* fp = fopen(inputFile.c_str(), "rb");

	if (!fp) return false;

	fseek(fp, 0, SEEK_END);
	long file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	auto* content = new ofbx::u8[file_size];
	fread(content, 1, file_size, fp);


    
	//ofbx::LoadFlags flags =
	//	//		ofbx::LoadFlags::IGNORE_MODELS |
	//	ofbx::LoadFlags::IGNORE_BLEND_SHAPES |
	//	ofbx::LoadFlags::IGNORE_CAMERAS |
	//	ofbx::LoadFlags::IGNORE_LIGHTS |
	//	//		ofbx::LoadFlags::IGNORE_TEXTURES |
	//	ofbx::LoadFlags::IGNORE_SKIN |
	//	ofbx::LoadFlags::IGNORE_BONES |
	//	ofbx::LoadFlags::IGNORE_PIVOTS |
	//	//		ofbx::LoadFlags::IGNORE_MATERIALS |
	//	ofbx::LoadFlags::IGNORE_POSES |
	//	ofbx::LoadFlags::IGNORE_VIDEOS |
	//	ofbx::LoadFlags::IGNORE_LIMBS |
	//	//		ofbx::LoadFlags::IGNORE_MESHES |
	//	ofbx::LoadFlags::IGNORE_ANIMATIONS;

	ofbx::LoadFlags flags = ofbx::LoadFlags::IGNORE_BLEND_SHAPES;
		

	ofbx::IScene* fbx_scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)flags);
	if (fbx_scene)
	{
		std::cout << "[INFO] FBX file loaded successfully" << std::endl;
	}
	else
	{
		std::cerr << "[ERROR] Failed to load FBX file " << ofbx::getError() << std::endl;
		return false;
	}

	auto meshCount = fbx_scene->getMeshCount();
	if (meshCount > 0)
	{
		std::cout << "[INFO] Found " << meshCount << " meshes" << std::endl;
		for (int i = 0; i < meshCount; i++)
		{
			const ofbx::Mesh* mesh = fbx_scene->getMesh(i);
			if (mesh)
			{
				std::cout << "[INFO] Add mesh " << mesh->name << std::endl;
				data.meshes.push_back(mesh);
			}
		}
	}
	else
	{
		std::cerr << "[ERROR] No meshes found in FBX file" << std::endl;
		return false;
	}


	// clean up
	delete[] content;
	fclose(fp);


    return true;
}


std::shared_ptr<hittable> fbx_mesh_loader::convert_model_from_file(fbx_mesh_data& data, randomizer& rnd, std::string name)
{
	return nullptr;
}