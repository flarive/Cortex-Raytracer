#include "fbx_mesh_loader.h"

#include "../materials/material.h"
#include "../materials/lambertian_material.h"
#include "../materials/phong_material.h"
#include "../materials/metal_material.h"
#include "../materials/dielectric_material.h"

#include "../textures/texture.h"
#include "../textures/solid_color_texture.h"
#include "../textures/image_texture.h"
#include "../textures/normal_texture.h"

#include "../cameras/perspective_camera.h"
#include "../cameras/orthographic_camera.h"

#include "../lights/omni_light.h"
#include "../lights/directional_light.h"
#include "../lights/spot_light.h"

#include "../primitives/rotate.h"
#include "../primitives/translate.h"
#include "../primitives/scale.h"

#include "helpers.h"
#include "math_utils.h"

#include <array>
#include <cmath> // For cos and sin
#include <fstream>

using case_insensitive_string = std::basic_string<char, case_insensitive_traits>;

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



	ofbx::LoadFlags flags = ofbx::LoadFlags::IGNORE_SKIN
		| ofbx::LoadFlags::IGNORE_BONES
		| ofbx::LoadFlags::IGNORE_PIVOTS
		| ofbx::LoadFlags::IGNORE_BLEND_SHAPES
		| ofbx::LoadFlags::IGNORE_POSES
		| ofbx::LoadFlags::IGNORE_VIDEOS
		| ofbx::LoadFlags::IGNORE_LIMBS
		| ofbx::LoadFlags::IGNORE_ANIMATIONS;

    


	data.scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)flags);
	if (data.scene)
	{
		std::cout << "[INFO] FBX file loaded successfully" << std::endl;
	}
	else
	{
		std::cerr << "[ERROR] Failed to load FBX file " << ofbx::getError() << std::endl;
		return false;
	}


    const ofbx::GlobalSettings* globalSettings = data.scene->getGlobalSettings();
    if (!globalSettings) {
        std::cerr << "[ERROR] GlobalSettings is not available" << std::endl;
        return false;
    }

    get_metadata(data.scene);


	// get meshes
	auto meshCount = data.scene->getMeshCount();
	if (meshCount > 0)
	{
		std::cout << "[INFO] Found " << meshCount << " meshes" << std::endl;
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

std::shared_ptr<hittable> fbx_mesh_loader::get_meshes(fbx_mesh_data& data, randomizer& rnd, const std::map<std::string, std::shared_ptr<material>>& scene_materials, const std::map<std::string, std::shared_ptr<texture>>& scene_textures, std::string name)
{
    hittable_list model_output;

    bool shade_smooth = true;

    const ofbx::IScene* scene = data.scene;
    const int mesh_count = scene->getMeshCount();

    std::cout << "[INFO] Building FBX model (" << mesh_count << " meshes found)" << std::endl;

    for (int mesh_idx = 0; mesh_idx < mesh_count; ++mesh_idx)
    {
        int triangle_idx = 0;
        
        const ofbx::Mesh* mesh = scene->getMesh(mesh_idx);

        if (!mesh)
            std::cout << "[WARNING] Can't get mesh: " << mesh->name << std::endl;

        const ofbx::GeometryData& geom = mesh->getGeometryData();

        const ofbx::Vec3Attributes positions = geom.getPositions();
        if (!positions.values || positions.count <= 0) {
            std::cout << "[WARNING] No vertex found for mesh: " << mesh->name << std::endl;
        }

        const ofbx::Vec3Attributes normals = geom.getNormals();
        if (!normals.values || normals.count <= 0) {
            std::cout << "[WARNING] No normals found for mesh: " << mesh->name << std::endl;
        }

        const ofbx::Vec2Attributes uvs = geom.getUVs();
        if (!uvs.values || uvs.count <= 0) {
            std::cout << "[WARNING] No UVs found for mesh: " << mesh->name << std::endl;
        }

        // Compute mesh material
        std::shared_ptr<material> mesh_material = get_mesh_materials(mesh, scene_materials, scene_textures);

        // Compute the local transformation matrix
        matrix4x4 mesh_transform = getGlobalTransform(mesh->getGlobalTransform());
        matrix4x4 normal_transform = mesh_transform.inverse().transpose(); // For normals

        ofbx::DVec3 translation, rotation, scale;
        decomposeDMatrix(mesh->getGlobalTransform(), translation, rotation, scale);

        std::cout << "[INFO] Mesh " << mesh->name << " (" << geom.getPartitionCount() << " partitions)" << std::endl;

        // verbose
        std::cout << "[INFO] Mesh " << mesh->name << " (" << translation.x << "/" << translation.y << "/" << translation.z << ")" << std::endl;


        for (int partition_idx = 0; partition_idx < geom.getPartitionCount(); ++partition_idx)
        {
            const ofbx::GeometryPartition& partition = geom.getPartition(partition_idx);

            std::cout << "[INFO] Mesh " << mesh->name << " polygon count " << partition.polygon_count << std::endl;

            for (int polygon_idx = 0; polygon_idx < partition.polygon_count; ++polygon_idx)
            {
                hittable_list shape_triangles;
                
                const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygon_idx];
                int vertex_count = polygon.vertex_count;

                int tri_indices[16];
                int tri_count = (vertex_count > 3) ? ofbx::triangulate(geom, polygon, tri_indices) : 1;

                for (int tri = 0; tri < tri_count; ++tri)
                {
                    std::array<vector3, 3> tri_v{};
                    std::array<vector3, 3> tri_vn{};
                    std::array<vector2, 3> tri_uv{};

                    for (int v = 0; v < 3; ++v)
                    {
                        int vertex_index = (vertex_count > 3) ? tri_indices[tri * 3 + v] : polygon.from_vertex + v;

                        assert(vertex_index >= 0);

                        // Transform vertex positions
                        ofbx::Vec3 pos = positions.get(vertex_index);
                        vector4 transformed_pos = mesh_transform * vector4(pos.x, pos.y, pos.z, 1.0);
                        tri_v[v] = vector3(transformed_pos.x, transformed_pos.y, transformed_pos.z);

                        // Transform normals (only apply rotation)
                        bool has_normals = normals.values != nullptr;
                        if (has_normals)
                        {
                            ofbx::Vec3 normal = normals.get(vertex_index);
                            vector4 transformed_normal = normal_transform * vector4(normal.x, normal.y, normal.z, 0.0);
                            tri_vn[v] = glm::normalize(vector3(transformed_normal.x, transformed_normal.y, transformed_normal.z));
                        }

                        // UVs
                        bool has_uvs = uvs.values != nullptr;
                        if (has_uvs)
                        {
                            ofbx::Vec2 uv = uvs.get(vertex_index);
                            tri_uv[v] = vector2(uv.x, uv.y);
                        }
                    }

                    // Tangent and Bitangent computation
                    std::array<vector3, 3> tri_tan{}, tri_bitan{};
                    computeTangentBasis(tri_v, tri_uv, tri_vn, tri_tan, tri_bitan);

                    shape_triangles.add(std::make_shared<triangle>(
                        mesh_idx, triangle_idx,
                        tri_v[0], tri_v[1], tri_v[2],
                        tri_vn[0], tri_vn[1], tri_vn[2],
                        tri_uv[0], tri_uv[1], tri_uv[2],
                        tri_tan[0], tri_tan[1], tri_tan[2],
                        tri_bitan[0], tri_bitan[1], tri_bitan[2],
                        shade_smooth, mesh_material));

                    // group all object triangles in a bvh node
                    model_output.add(std::make_shared<bvh_node>(shape_triangles, rnd, mesh->name));

                    triangle_idx++;
                }
            }

            std::cout << "[INFO] Parsing fbx file (object name " << mesh->name << " / " << positions.values_count << " vertex / " << partition.polygon_count << " faces)" << std::endl;
        }
    }

    // look for embbeded textures in fbx file
    //if (scene->getEmbeddedDataCount() > 0)
    //{
    //    for (int emdata_idx = 0; emdata_idx < scene->getEmbeddedDataCount(); ++emdata_idx)
    //    {
    //        const ofbx::IElementProperty* prop = scene->getEmbeddedBase64Data(emdata_idx);
    //        if (prop)
    //        {
    //        }
    //    }
    //}

    // group all objects in the .fbx file in a single bvh node
    return std::make_shared<bvh_node>(model_output, rnd, name);
}

std::vector<std::shared_ptr<camera>> fbx_mesh_loader::get_cameras(fbx_mesh_data& data, double aspectRatio, short int index)
{
    std::vector<std::shared_ptr<camera>> cameras;

    const ofbx::IScene* scene = data.scene;
    const int camera_count = scene->getCameraCount();

    std::cout << "[INFO] Building FBX model (" << camera_count << " cameras found)" << std::endl;

    for (int cam_idx = 0; cam_idx < camera_count; ++cam_idx)
    {
        if (cam_idx != index)
            continue;

        const ofbx::Camera* ofbxcam = scene->getCamera(cam_idx);
        if (ofbxcam)
        {
            std::shared_ptr<camera> c = nullptr;

            double diagonal = ofbxcam->getFocalLength(); // Example: 35mm sensor diagonal (lens in 3ds max)
            sensor_dimensions dimensions = calculateSensorDimensions(diagonal, aspectRatio);

            if (ofbxcam->getProjectionType() == ofbx::Camera::ProjectionType::ORTHOGRAPHIC)
            {
                // Orthographic camera
                c = std::make_shared<orthographic_camera>();
                c->ortho_height = calculateOrthoHeight(ofbxcam, aspectRatio);
                c->vfov = 0;
                c->is_orthographic = true;
            }
            else
            {
                // Perspective camera
                c = std::make_shared<perspective_camera>();
                c->vfov = calculateVerticalFOV(ofbxcam, dimensions.height);
                c->ortho_height = 0;
                c->is_orthographic = false;
            }

            c->aspect_ratio = aspectRatio;

            // Extract the camera's local transformation matrix
            const ofbx::DMatrix cam_transform = ofbxcam->getGlobalTransform();

            ofbx::DVec3 translation, rotation, scale;
            //decomposeDMatrix2(cam_transform, translation, rotation, scale, RotationOrder::ZYX);
            decomposeDMatrix(cam_transform, translation, rotation, scale);



            // Extract position (lookFrom)
            vector3 look_from(translation.x, translation.y, translation.z);

            // Interest position (lookAt)
            auto ip = ofbxcam->getInterestPosition();
            vector3 look_at = vector3(ip.x, ip.y, ip.z);

            // Default up axis
            vector3 up_axis = extractUpAxis(ofbxcam->getLocalTransform());

            // Assign the extracted values to the camera config
            c->lookfrom = look_from;
            c->lookat = vector3(look_at.x, look_at.y, look_at.z);
            c->vup = vector3(up_axis.x, up_axis.y, up_axis.z);

            // Additional camera properties
            c->defocus_angle = 0.0;
            c->focus_dist = ofbxcam->getFocalLength(); // lens in 3ds max free camera, not sure ??????????

            std::cout << "[INFO] Camera " << cam_idx
                << " - LookFrom: (" << c->lookfrom.x << ", " << c->lookfrom.y << ", " << c->lookfrom.z << ")"
                << " LookAt: (" << c->lookat.x << ", " << c->lookat.y << ", " << c->lookat.z << ")"
                << " UpAxis: (" << c->vup.x << ", " << c->vup.y << ", " << c->vup.z << ")"
                << std::endl;

            cameras.emplace_back(c);
        }
    }

    return cameras;
}

std::vector<std::shared_ptr<light>> fbx_mesh_loader::get_lights(fbx_mesh_data& data, short int index)
{
    std::vector<std::shared_ptr<light>> lights;

    const ofbx::IScene* scene = data.scene;
    const int light_count = scene->getLightCount();

    std::cout << "[INFO] Building FBX model (" << light_count << " lights found)" << std::endl;

    for (int light_idx = 0; light_idx < light_count; ++light_idx)
    {
        if (index >= 0 && light_idx != index)
            continue;

        const ofbx::Light* ofbxlight = scene->getLight(light_idx);
        if (ofbxlight)
        {
            std::shared_ptr<light> l = nullptr;

            // Extract the light's local transformation matrix
            const ofbx::DMatrix light_transform = ofbxlight->getGlobalTransform();

            vector3 pos = vector3(0, 0, 0);
            double intensity = ofbxlight->getIntensity() / 5.0; // multiplier in 3ds max
            color rgb = color(ofbxlight->getColor().r, ofbxlight->getColor().g, ofbxlight->getColor().b);

            if (ofbxlight->getLightType() == ofbx::Light::LightType::POINT)
            {
                double radius = 3.0 * 5.0; // 1.0;

                apply_transformation_to_omni(light_transform, pos);

                l = std::make_shared<omni_light>(pos, radius, intensity, rgb, ofbxlight->name, true);
            }
            else if (ofbxlight->getLightType() == ofbx::Light::LightType::DIRECTIONAL)
            {
                vector3 u(200, 0.0, 0.0);
                vector3 v(0.0, 0.0, 200);

                // Apply transformation
                apply_transformation_to_directional(light_transform, pos, u, v);

                l = std::make_shared<directional_light>(pos, u, v, intensity / 3.0, rgb, ofbxlight->name, true);
            }
            else if (ofbxlight->getLightType() == ofbx::Light::LightType::SPOT)
            {
                //ofbxlight->spo
                //l = std::make_shared<spot_light>(pos, u, v, intensity, rgb, ofbxlight->name, false);
            }

            lights.emplace_back(l);
        }
    }

    return lights;
}

double fbx_mesh_loader::calculateVerticalFOV(const ofbx::Object* camera, double sensorHeight = 24.0f) // 35mm camera
{
    if (!camera || camera->getType() != ofbx::Object::Type::CAMERA)
        std::cerr << "[ERROR] Invalid or non-camera object passed" << std::endl;

    const ofbx::Camera* cam = static_cast<const ofbx::Camera*>(camera);

    // Get the focal length
    double focalLength = cam->getFocalLength(); // lens in 3ds max
    if (focalLength <= 0.0) {
        std::cerr << "[WARNING] Invalid focal length value" << std::endl;
    }

    // Calculate vertical FOV
    double verticalFOVRadians = 2.0 * atan(sensorHeight / (2.0 * focalLength));

    // Convert to degrees
    double verticalFOV = verticalFOVRadians * (180.0 / M_PI);

    return verticalFOV;
}

fbx_mesh_loader::sensor_dimensions fbx_mesh_loader::calculateSensorDimensions(double diagonal, double aspectRatio)
{
    double height = diagonal / std::sqrt(1.0 + aspectRatio * aspectRatio);
    double width = height * aspectRatio;
    return { width, height };
}

double fbx_mesh_loader::calculateOrthoHeight(const ofbx::Camera* camera, double aspectRatio)
{
    if (camera->getProjectionType() == ofbx::Camera::ProjectionType::ORTHOGRAPHIC)
    {
        double orthoScale = camera->getOrthoZoom(); // ortho zoom not exported by 3ds max fbx exporter (always default value 1.0)
        //double orthoHeight = 2.0 * orthoScale;
        double orthoHeight = camera->getFocalLength() * aspectRatio * 1.5; // probably false but better than using getOrthoZoom()
        //double orthoWidth = orthoHeight * aspectRatio;

        return orthoHeight;
    }

    return 0;
}

vector3 fbx_mesh_loader::extractUpAxis(const ofbx::DMatrix& cam_transform)
{
    // The up axis is the second column of the local transform matrix
    return vector3(
        cam_transform.m[1 * 4 + 0], // X component of up axis
        cam_transform.m[1 * 4 + 1], // Y component of up axis
        cam_transform.m[1 * 4 + 2] // Z component of up axis
    );
}

std::shared_ptr<material> fbx_mesh_loader::get_mesh_materials(const ofbx::Mesh* mesh, const std::map<std::string, std::shared_ptr<material>>& scene_materials, const std::map<std::string, std::shared_ptr<texture>>& scene_textures)
{
    std::shared_ptr<texture> tex_diffuse = nullptr;
    std::shared_ptr<texture> tex_specular = nullptr;
    std::shared_ptr<texture> tex_bump = nullptr;
    std::shared_ptr<texture> tex_normal = nullptr;
    std::shared_ptr<texture> tex_displace = nullptr;
    std::shared_ptr<texture> tex_alpha = nullptr;
    std::shared_ptr<texture> tex_emissive = nullptr;

    auto shaderModel = material_shader_model::Undefined;

    color ambientColor{};
    double ambientFactor = 0.0;

    color diffuseColor{};
    double diffuseFactor = 0.0;

    color specularColor{};
    double specularFactor = 0.0;

    //double bumpFactor = 0.0;

    double shininess = 0.0;
    double shininessFactor = 0.0;
    double opacity = 0.0;

    double fuzziness = 0.0;
    double index_of_refraction = 0.0;

    // get fbx mesh materials
    for (int material_idx = 0; material_idx < mesh->getMaterialCount(); ++material_idx)
    {
        const ofbx::Material* mat = mesh->getMaterial(material_idx);
        if (mat)
        {
            std::string materialName = std::string(mat->name);

            // try to get a scene override material (will be taken instead of fbx material)
            if (!materialName.empty())
            {
                auto it = scene_materials.find(materialName);
                if (it != scene_materials.end())
                {
                    // if key is found
                    std::cout << "[INFO] Material " << materialName << " (" << to_string(it->second) << ")" << " overrided in scene file" << std::endl;
                    return it->second;
                }
            }

            ambientColor = to_color(mat->getAmbientColor());
            ambientFactor = mat->getAmbientFactor(); // ambient color amount in 3ds max (between 0.0 and 1.0 here)

            diffuseColor = to_color(mat->getDiffuseColor());
            diffuseFactor = mat->getDiffuseFactor(); // diffuse color amount in 3ds max (between 0.0 and 1.0 here)

            specularColor = to_color(mat->getSpecularColor());
            specularFactor = mat->getSpecularFactor(); // specular color amount in 3ds max (between 0.0 and 1.0 here)

            shininess = scaleValue(mat->getShininess(), 1.0, 1024.0, 0.0, 10.0); // glossiness in 3ds max (between 1.0 and 1024.0 here)
            opacity = mat->getOpacity(); // opacity in 3ds max (between 0.0 and 1.0 here)

            double bumpFactor = mat->getBumpFactor(); // bump/normal amount in 3ds max (between 0.0 and 1.0 here)


            // get shading model from material name, default is phong
            if (case_insensitive_string(materialName.data(), materialName.size()).starts_with("lambert"))
                shaderModel = material_shader_model::Lambertian;
            else if (case_insensitive_string(materialName.data(), materialName.size()).starts_with("metal"))
                shaderModel = material_shader_model::Metal;
            else if (case_insensitive_string(materialName.data(), materialName.size()).starts_with("glass"))
                shaderModel = material_shader_model::Dielectric;
            else
                shaderModel = material_shader_model::Phong;


            

            std::cout << "[INFO] Material " << materialName << " (" << shaderModel << ")" << std::endl;

            // Retrieve material textures
            tex_diffuse = get_texture(mat, ofbx::Texture::DIFFUSE, scene_textures, diffuseFactor);
            tex_specular = get_texture(mat, ofbx::Texture::SPECULAR, scene_textures, specularFactor);
            tex_normal = get_texture(mat, ofbx::Texture::NORMAL, scene_textures, bumpFactor);
        }
        else
            std::cout << "[WARNING] No material assigned to this mesh." << std::endl;
    }


    std::shared_ptr<material> mesh_material = nullptr;

    if (shaderModel == material_shader_model::Lambertian)
    {
        // lambert
        if (tex_diffuse)
            mesh_material = std::make_shared<lambertian_material>(tex_diffuse);
        else
            mesh_material = std::make_shared<lambertian_material>(std::make_shared<solid_color_texture>(ambientColor));
    }
    else if (shaderModel == material_shader_model::Metal)
    {
        // metal
        if (tex_diffuse)
            mesh_material = std::make_shared<metal_material>(tex_diffuse, fuzziness);
        else
            mesh_material = std::make_shared<metal_material>(std::make_shared<solid_color_texture>(ambientColor), fuzziness);
    }
    else if (shaderModel == material_shader_model::Dielectric)
    {
        // glass
        if (tex_diffuse)
            mesh_material = std::make_shared<dielectric_material>(index_of_refraction, tex_diffuse);
        else
            mesh_material = std::make_shared<dielectric_material>(index_of_refraction, std::make_shared<solid_color_texture>(ambientColor));
    }
    else
    {
        // phong
        if (tex_diffuse)
            mesh_material = std::make_shared<phong_material>(tex_diffuse, tex_specular, tex_bump, tex_normal, tex_displace, tex_alpha, tex_emissive, ambientColor, shininess);
        else
            mesh_material = std::make_shared<phong_material>(std::make_shared<solid_color_texture>(ambientColor), nullptr, ambientColor, shininess);
    }
    
    return mesh_material;
}

std::shared_ptr<texture> fbx_mesh_loader::get_texture(const ofbx::Material* mat, ofbx::Texture::TextureType textureKind, const std::map<std::string, std::shared_ptr<texture>>& scene_textures, double amount)
{
    std::shared_ptr<texture> tex = nullptr;
    
    if (const ofbx::Texture* texture = mat->getTexture(textureKind))
    {
        std::string textureName = std::string(texture->name);

        std::string textureKindName;
        if (textureKind == ofbx::Texture::DIFFUSE) textureKindName = "Diffuse";
        else if (textureKind == ofbx::Texture::SPECULAR) textureKindName = "Specular";
        else if (textureKind == ofbx::Texture::NORMAL) textureKindName = "Normal";

        // try to get a scene override texture (will be taken instead of fbx texture)
        if (!textureName.empty())
        {
            auto it = scene_textures.find(textureName);
            if (it != scene_textures.end())
            {
                // if key is found
                std::cout << "[INFO] " << textureKindName << " texture " << textureName << " overrided in scene file" << std::endl;
                tex = it->second;
                return tex;
            }
        }

        // no override found
        const ofbx::DataView embeddedData = texture->getEmbeddedData();
        if (embeddedData.begin && embeddedData.end && (embeddedData.end > embeddedData.begin))
        {
            size_t dataSize = embeddedData.end - embeddedData.begin;

            // Save the texture data to a file
            //std::ofstream outFile(outputPath, std::ios::binary);
            //outFile.write(reinterpret_cast<const char*>(embeddedData.begin), dataSize);
            //outFile.close();

            std::cout << "Embedded data found for texture " << texture->name << std::endl;
        }
        else
        {
            const ofbx::DataView dv = texture->getFileName();
            char buffer[256] = {}; // Ensure the buffer is large enough
            dv.toString(buffer);  // Converts DataView to a null-terminated string
            std::cout << "[INFO] " << textureKindName << " texture " << buffer << std::endl;

            if (textureKind == ofbx::Texture::NORMAL)
                tex = std::make_shared<normal_texture>(std::make_shared<image_texture>(std::string(buffer)), amount);
            else
                tex = std::make_shared<image_texture>(std::string(buffer));
        }
    }

    return tex;
}

void fbx_mesh_loader::computeTangentBasis(std::array<vector3, 3>& vertices, std::array<vector2, 3>& uvs, std::array<vector3, 3>& normals, std::array<vector3, 3>& tangents, std::array<vector3, 3>& bitangents)
{
	for (size_t i = 0; i < vertices.size(); i += 3) {
		vector3 deltaPos1 = vertices[1] - vertices[0];
		vector3 deltaPos2 = vertices[2] - vertices[0];

		vector2 deltaUV1 = uvs[1] - uvs[0];
		vector2 deltaUV2 = uvs[2] - uvs[0];

		double r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		vector3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		vector3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		tangents[0] = tangents[1] = tangents[2] = tangent;
		bitangents[0] = bitangents[1] = bitangents[2] = bitangent;
	}
}

// Helper function to create a 4x4 transformation matrix
matrix4x4 fbx_mesh_loader::getGlobalTransform(const ofbx::DMatrix matrix)
{
    matrix4x4 transform;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            transform.m[i][j] = matrix.m[i * 4 + j];
        }
    }

    return transform;
}

// Convert coordinates from Y-up to Z-up
vector3 fbx_mesh_loader::convertToMaxSystem(const vector3& openfbxVector)
{
    return vector3(
        openfbxVector.x,  // X remains the same
        openfbxVector.z,  // Z in FBX becomes Y in 3ds Max
        -openfbxVector.y  // Y in FBX becomes -Z in 3ds Max
    );
}

vector3 fbx_mesh_loader::convertFromMaxSystem(const vector3& maxSystemVector)
{
    return vector3(
        maxSystemVector.x,  // X remains the same
        -maxSystemVector.z, // -Z in 3ds Max becomes Y in FBX
        maxSystemVector.y   // Y in 3ds Max becomes Z in FBX
    );
}

vector3 fbx_mesh_loader::convertToBlenderSystem(const vector3& fbxVector)
{
    return vector3(
        fbxVector.x,   // X remains the same
        -fbxVector.z,  // Z in FBX becomes -Y in Blender
        fbxVector.y    // Y in FBX becomes Z in Blender
    );
}

vector3 fbx_mesh_loader::convertFromBlenderSystem(const vector3& blenderVector)
{
    return vector3(
        blenderVector.x,  // X remains the same
        blenderVector.z,  // Z in Blender becomes Y in FBX
        -blenderVector.y  // -Y in Blender becomes Z in FBX
    );
}

// Helper function to compute the magnitude of a 3D vector
double fbx_mesh_loader::vectorLength(const ofbx::DVec3& vec)
{
    return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

void fbx_mesh_loader::decomposeDMatrix2(const ofbx::DMatrix& matrix, ofbx::DVec3& translation, ofbx::DVec3& rotation, ofbx::DVec3& scale, RotationOrder order)
{
    // Extract translation
    translation.x = matrix.m[12];
    translation.y = matrix.m[13];
    translation.z = matrix.m[14];

    // Extract scale
    scale.x = sqrt(matrix.m[0] * matrix.m[0] + matrix.m[1] * matrix.m[1] + matrix.m[2] * matrix.m[2]);
    scale.y = sqrt(matrix.m[4] * matrix.m[4] + matrix.m[5] * matrix.m[5] + matrix.m[6] * matrix.m[6]);
    scale.z = sqrt(matrix.m[8] * matrix.m[8] + matrix.m[9] * matrix.m[9] + matrix.m[10] * matrix.m[10]);

    // Normalize matrix columns to remove scale
    ofbx::DMatrix rotationMatrix = matrix;
    rotationMatrix.m[0] /= scale.x;
    rotationMatrix.m[1] /= scale.x;
    rotationMatrix.m[2] /= scale.x;
    rotationMatrix.m[4] /= scale.y;
    rotationMatrix.m[5] /= scale.y;
    rotationMatrix.m[6] /= scale.y;
    rotationMatrix.m[8] /= scale.z;
    rotationMatrix.m[9] /= scale.z;
    rotationMatrix.m[10] /= scale.z;

    // Extract rotation based on the specified order
    switch (order)
    {
    case RotationOrder::XYZ:
        rotation.y = asin(-rotationMatrix.m[2]);
        if (cos(rotation.y) != 0) {
            rotation.x = atan2(rotationMatrix.m[6], rotationMatrix.m[10]);
            rotation.z = atan2(rotationMatrix.m[1], rotationMatrix.m[0]);
        }
        else {
            rotation.x = atan2(-rotationMatrix.m[9], rotationMatrix.m[5]);
            rotation.z = 0.0;
        }
        break;

    case RotationOrder::ZYX:
        rotation.x = asin(-rotationMatrix.m[6]);
        if (cos(rotation.x) != 0) {
            rotation.y = atan2(rotationMatrix.m[2], rotationMatrix.m[10]);
            rotation.z = atan2(rotationMatrix.m[4], rotationMatrix.m[5]);
        }
        else {
            rotation.y = atan2(-rotationMatrix.m[8], rotationMatrix.m[0]);
            rotation.z = 0.0;
        }
        break;

        // Handle other orders similarly...
    }

    // Convert radians to degrees
    rotation.x = radians_to_degrees(rotation.x);
    rotation.y = radians_to_degrees(rotation.y);
    rotation.z = radians_to_degrees(rotation.z);
}


// Function to decompose a DMatrix into translation, rotation, and scale
void fbx_mesh_loader::decomposeDMatrix(const ofbx::DMatrix& matrix, ofbx::DVec3& translation, ofbx::DVec3& rotation, ofbx::DVec3& scale)
{
    // Extract translation
    translation.x = matrix.m[12];
    translation.y = matrix.m[13];
    translation.z = matrix.m[14];

    // Extract scale
    scale.x = sqrt(matrix.m[0] * matrix.m[0] + matrix.m[1] * matrix.m[1] + matrix.m[2] * matrix.m[2]);
    scale.y = sqrt(matrix.m[4] * matrix.m[4] + matrix.m[5] * matrix.m[5] + matrix.m[6] * matrix.m[6]);
    scale.z = sqrt(matrix.m[8] * matrix.m[8] + matrix.m[9] * matrix.m[9] + matrix.m[10] * matrix.m[10]);

    // Normalize matrix columns to remove scale
    ofbx::DMatrix rotationMatrix = matrix;
    rotationMatrix.m[0] /= scale.x;
    rotationMatrix.m[1] /= scale.x;
    rotationMatrix.m[2] /= scale.x;
    rotationMatrix.m[4] /= scale.y;
    rotationMatrix.m[5] /= scale.y;
    rotationMatrix.m[6] /= scale.y;
    rotationMatrix.m[8] /= scale.z;
    rotationMatrix.m[9] /= scale.z;
    rotationMatrix.m[10] /= scale.z;

    // Extract rotation
    rotation.y = asin(-rotationMatrix.m[2]); // Rotation around Y-axis
    if (cos(rotation.y) != 0) {
        rotation.x = atan2(rotationMatrix.m[6], rotationMatrix.m[10]); // Rotation around X-axis
        rotation.z = atan2(rotationMatrix.m[1], rotationMatrix.m[0]);  // Rotation around Z-axis
    }
    else {
        rotation.x = atan2(-rotationMatrix.m[9], rotationMatrix.m[5]);
        rotation.z = 0.0;
    }

    // Convert radians to degrees
    rotation.x = radians_to_degrees(rotation.x);
    rotation.y = radians_to_degrees(rotation.y);
    rotation.z = radians_to_degrees(rotation.z);
}

vector3 fbx_mesh_loader::transform_vector(const ofbx::DMatrix& matrix, const vector3& vec)
{
    vector3 result;
    result.x = matrix.m[0] * vec.x + matrix.m[4] * vec.y + matrix.m[8] * vec.z + matrix.m[12];
    result.y = matrix.m[1] * vec.x + matrix.m[5] * vec.y + matrix.m[9] * vec.z + matrix.m[13];
    result.z = matrix.m[2] * vec.x + matrix.m[6] * vec.y + matrix.m[10] * vec.z + matrix.m[14];
    return result;
}

void fbx_mesh_loader::apply_transformation_to_omni(const ofbx::DMatrix& matrix, vector3& pos)
{
    // Transform position (includes translation)
    pos = transform_vector(matrix, pos);
}

void fbx_mesh_loader::apply_transformation_to_directional(const ofbx::DMatrix& matrix, vector3& pos, vector3& u, vector3& v)
{
    // Transform position (includes translation)
    pos = transform_vector(matrix, pos);

    // Transform u and v as directions
    vector3 origin = { 0, 0, 0 };
    vector3 transformed_u = transform_vector(matrix, u) - transform_vector(matrix, origin);
    vector3 transformed_v = transform_vector(matrix, v) - transform_vector(matrix, origin);

    // Center the quad around the transformed position
    u = transformed_u * 0.5;
    v = transformed_v * 0.5;

    // Update position to the center of the quad
    pos = pos - u - v; // Adjust position to the new bottom-left corner
}

void fbx_mesh_loader::get_metadata(const ofbx::IScene* scene)
{
    const ofbx::Headers* headers = scene->getHeaders();
    if (headers)
    {
        int fbxHeaderVersion = headers->fbxHeaderVersion;
        int fbxVersion = headers->fbxVersion;
        int encryptionType = headers->encryptionType;
        char* creator = headers->creator;
        char* creationTime = headers->creationTime;

        char* documentUrl = headers->documentUrl;
        char* srcDocumentUrl = headers->srcDocumentUrl;

        char* originalApplicationVendor = headers->originalApplicationVendor;
        char* originalApplicationName = headers->originalApplicationName;
        char* originalApplicationVersion = headers->originalApplicationVersion;
        char* originalDateTimeGMT = headers->originalDateTimeGMT;
        char* originalFilename = headers->originalFilename;
        char* originalApplicationActiveProject = headers->originalApplicationActiveProject;
        char* originalApplicationNativeFile = headers->originalApplicationNativeFile;

        char* lastSavedApplicationVendor = headers->lastSavedApplicationVendor;
        char* lastSavedApplicationName = headers->lastSavedApplicationName;
        char* lastSavedApplicationVersion = headers->lastSavedApplicationVersion;
        char* lastSavedDateTimeGMT = headers->lastSavedDateTimeGMT;
    }
}

color fbx_mesh_loader::to_color(ofbx::Color rgb)
{
    return color(rgb.r, rgb.g, rgb.b);
}

double fbx_mesh_loader::scaleValue(double input, double sourceMin, double sourceMax, double targetMin, double targetMax)
{
    // Ensure input is within the source range
    if (input < sourceMin) input = sourceMin;
    if (input > sourceMax) input = sourceMax;

    // Perform the scaling
    return targetMin + (input - sourceMin) * (targetMax - targetMin) / (sourceMax - sourceMin);
}