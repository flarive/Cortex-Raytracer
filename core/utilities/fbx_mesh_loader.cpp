#include "fbx_mesh_loader.h"

#include "../materials/material.h"
#include "../materials/lambertian_material.h"
#include "../materials/phong_material.h"
#include "../materials/metal_material.h"

#include "../textures/texture.h"
#include "../textures/solid_color_texture.h"
#include "../textures/image_texture.h"

#include "../cameras/perspective_camera.h"
#include "../cameras/orthographic_camera.h"

#include "../lights/omni_light.h"
#include "../lights/directional_light.h"
#include "../lights/spot_light.h"

#include "../primitives/rotate.h"
#include "../primitives/translate.h"
#include "../primitives/scale.h"

#include "helpers.h"

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


std::shared_ptr<hittable> fbx_mesh_loader::get_meshes(fbx_mesh_data& data, randomizer& rnd, std::string name)
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
        std::shared_ptr<material> mesh_material = get_mesh_materials(mesh);

        // Compute the local transformation matrix
        matrix4x4 transform = getGlobalTransform(mesh);
        matrix4x4 normal_transform = transform.inverse().transpose(); // For normals

        std::cout << "[INFO] Mesh " << mesh->name << " (" << geom.getPartitionCount() << " partitions)" << std::endl;
        

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
                        vector4 transformed_pos = transform * vector4(pos.x, pos.y, pos.z, 1.0);
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
                c->vfov = getVerticalFOV(ofbxcam, dimensions.height);
                c->ortho_height = 0;
                c->is_orthographic = false;
            }

            c->aspect_ratio = aspectRatio;

            // Extract the camera's local transformation matrix
            const ofbx::DMatrix cam_transform = ofbxcam->getGlobalTransform();

            ofbx::DVec3 translation, rotation, scale;
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
            c->focus_dist = ofbxcam->getFocalLength(); // lens in 3ds max free camera

            //std::cout << "[INFO] Camera " << cam_idx
            //    << " - LookFrom: (" << c->lookfrom.x << ", " << c->lookfrom.y << ", " << c->lookfrom.z << ")"
            //    << " LookAt: (" << c->lookat.x << ", " << c->lookat.y << ", " << c->lookat.z << ")"
            //    << " UpAxis: (" << c->vup.x << ", " << c->vup.y << ", " << c->vup.z << ")"
            //    << std::endl;

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

double fbx_mesh_loader::getVerticalFOV(const ofbx::Object* camera, double sensorHeight = 24.0f) // 35mm camera
{
    if (!camera || camera->getType() != ofbx::Object::Type::CAMERA) {
        throw std::invalid_argument("Invalid or non-camera object passed.");
    }

    const ofbx::Camera* cam = static_cast<const ofbx::Camera*>(camera);

    // Get the focal length
    double focalLength = cam->getFocalLength(); // lens in 3ds max
    if (focalLength <= 0.0) {
        throw std::runtime_error("Invalid focal length value.");
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

std::shared_ptr<material> fbx_mesh_loader::get_mesh_materials(const ofbx::Mesh* mesh)
{
    std::shared_ptr<texture> tex_diffuse = nullptr;
    std::shared_ptr<texture> tex_specular = nullptr;

    auto shaderModel = materialShaderModel::Undefined;

    // get mesh materials
    for (int material_idx = 0; material_idx < mesh->getMaterialCount(); ++material_idx)
    {
        const ofbx::Material* mat = mesh->getMaterial(material_idx);
        if (mat)
        {
            ofbx::Color ambientColor = mat->getAmbientColor();
            double ambientFactor = mat->getAmbientFactor();

            ofbx::Color diffuseColor = mat->getDiffuseColor();
            double diffuseFactor = mat->getDiffuseFactor();

            ofbx::Color specularColor = mat->getSpecularColor();
            double specularFactor = mat->getSpecularFactor();

            double shininess = mat->getShininess();
            double shininessFactor = mat->getShininessExponent();

            // opacity ?

            std::string materialName = std::string(mat->name);
            
            // get shading model from material name, default is phong
            if (case_insensitive_string(materialName.data(), materialName.size()).starts_with("lambert"))
                shaderModel = materialShaderModel::Lambertian;
            else
                shaderModel = materialShaderModel::Phong;

            std::cout << "[INFO] Material: " << materialName.c_str() << " (" << (shaderModel == materialShaderModel::Lambertian ? "Lambert" : "Phong") << ")" << std::endl;

            // Retrieve material properties
            if (const ofbx::Texture* diffuseTexture = mat->getTexture(ofbx::Texture::DIFFUSE))
            {
                const ofbx::DataView dv = diffuseTexture->getFileName();
                char buffer[256] = {}; // Ensure the buffer is large enough
                dv.toString(buffer);  // Converts DataView to a null-terminated string
                tex_diffuse = std::make_shared<image_texture>(std::string(buffer));
                std::cout << "[INFO] Diffuse texture " << buffer << std::endl;
            }

            if (const ofbx::Texture* specularTexture = mat->getTexture(ofbx::Texture::SPECULAR))
            {
                const ofbx::DataView dv = specularTexture->getFileName();
                char buffer[256] = {}; // Ensure the buffer is large enough
                dv.toString(buffer);  // Converts DataView to a null-terminated string
                tex_specular = std::make_shared<image_texture>(std::string(buffer));
                std::cout << "[INFO] Specular texture " << buffer << std::endl;
            }

            //if (const ofbx::Texture* normalMap = mat->getTexture(ofbx::Texture::NORMAL)) {
            //    const char* textureFileName = normalMap->getRelativeFileName();
            //    std::cout << "    [INFO] Normal Map: " << textureFileName << std::endl;
            //}
        }
        else
            std::cout << "[WARNING] No material assigned to this mesh." << std::endl;
    }


    std::shared_ptr<material> mesh_material = nullptr;

    if (shaderModel == materialShaderModel::Lambertian)
    {
        // lambert
        if (tex_diffuse)
            mesh_material = std::make_shared<lambertian_material>(tex_diffuse);
        else
            mesh_material = std::make_shared<lambertian_material>(std::make_shared<solid_color_texture>(0.6, 0.6, 0.6)); // default gray color material
    }
    else if (shaderModel == materialShaderModel::Metal)
    {
        // metal
        if (tex_diffuse)
            mesh_material = std::make_shared<metal_material>(tex_diffuse, 0.0);
        else
            mesh_material = std::make_shared<metal_material>(color(0.6, 0.6, 0.6), 0.0); // default gray color material
    }
    else
    {
        // phong
        if (tex_diffuse)
            mesh_material = std::make_shared<phong_material>(tex_diffuse, tex_specular, color(0.0, 0.0, 0.0), 10.0);
        else
            mesh_material = std::make_shared<lambertian_material>(std::make_shared<solid_color_texture>(0.6, 0.6, 0.6)); // default gray color material
    }
    

    return mesh_material;
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
matrix4x4 fbx_mesh_loader::getGlobalTransform(const ofbx::Mesh* mesh)
{
    ofbx::DMatrix matrix = mesh->getGlobalTransform();

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

// Helper function to compute the magnitude of a 3D vector
double fbx_mesh_loader::vectorLength(const ofbx::DVec3& vec)
{
    return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
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
    const double RAD_TO_DEG = 180.0 / M_PI;
    rotation.x *= RAD_TO_DEG;
    rotation.y *= RAD_TO_DEG;
    rotation.z *= RAD_TO_DEG;
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
    const ofbx::IElement* elem = scene->getRootElement();
    if (elem)
    {
        auto ppp = elem->getFirstChild();
        if (ppp)
        {
            auto aaa = ppp->getFirstProperty();

            auto hhh = ppp->getSibling();

            int z = 0;
        }

    }
}



