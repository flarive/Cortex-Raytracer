#include "fbx_mesh_loader.h"

#include "../materials/lambertian_material.h"
#include "../textures/image_texture.h"

#include "../cameras/perspective_camera.h"
#include "../cameras/orthographic_camera.h"

#include "../primitives/rotate.h"
#include "../primitives/translate.h"
#include "../primitives/scale.h"



#include <filesystem>
#include <array>
#include <cmath> // For cos and sin

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


std::shared_ptr<hittable> fbx_mesh_loader::convert_model_from_file(fbx_mesh_data& data, randomizer& rnd, std::string name)
{
    hittable_list model_output;

    bool shade_smooth = true;
    auto tex = std::make_shared<image_texture>("../../data/models/giantbug_diffuse.jpg");
    auto model_material = std::make_shared<lambertian_material>(tex);

    const ofbx::IScene* scene = data.scene;
    const int mesh_count = scene->getMeshCount();

    std::cout << "[INFO] Building FBX model (" << mesh_count << " meshes found)" << std::endl;

    for (int mesh_idx = 0; mesh_idx < mesh_count; ++mesh_idx)
    {
        const ofbx::Mesh* mesh = scene->getMesh(mesh_idx);
        const ofbx::GeometryData& geom = mesh->getGeometryData();
        const ofbx::Vec3Attributes positions = geom.getPositions();
        const ofbx::Vec3Attributes normals = geom.getNormals();
        const ofbx::Vec2Attributes uvs = geom.getUVs();

        // Compute the local transformation matrix
        matrix4x4 transform = getLocalTransform(mesh);
        matrix4x4 normal_transform = transform.inverse().transpose(); // For normals

        hittable_list shape_triangles;

        for (int partition_idx = 0; partition_idx < geom.getPartitionCount(); ++partition_idx)
        {
            const ofbx::GeometryPartition& partition = geom.getPartition(partition_idx);

            for (int polygon_idx = 0; polygon_idx < partition.polygon_count; ++polygon_idx)
            {
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

                        // Transform vertex positions
                        ofbx::Vec3 pos = positions.get(vertex_index);
                        vector4 transformed_pos = transform * vector4(pos.x, pos.y, pos.z, 1.0);
                        tri_v[v] = vector3(transformed_pos.x, transformed_pos.y, transformed_pos.z);

                        // Transform normals (only apply rotation)
                        if (normals.values)
                        {
                            ofbx::Vec3 normal = normals.get(vertex_index);
                            vector4 transformed_normal = normal_transform * vector4(normal.x, normal.y, normal.z, 0.0);
                            tri_vn[v] = glm::normalize(vector3(transformed_normal.x, transformed_normal.y, transformed_normal.z));
                        }

                        // UVs
                        if (uvs.values)
                        {
                            ofbx::Vec2 uv = uvs.get(vertex_index);
                            tri_uv[v] = vector2(uv.x, uv.y);
                        }
                    }

                    // Tangent and Bitangent computation
                    std::array<vector3, 3> tri_tan{}, tri_bitan{};
                    computeTangentBasis(tri_v, tri_uv, tri_vn, tri_tan, tri_bitan);

                    shape_triangles.add(std::make_shared<triangle>(
                        mesh_idx, partition_idx,
                        tri_v[0], tri_v[1], tri_v[2],
                        tri_vn[0], tri_vn[1], tri_vn[2],
                        tri_uv[0], tri_uv[1], tri_uv[2],
                        tri_tan[0], tri_tan[1], tri_tan[2],
                        tri_bitan[0], tri_bitan[1], tri_bitan[2],
                        shade_smooth, model_material));
                }
            }

            model_output.add(std::make_shared<bvh_node>(shape_triangles, rnd, mesh->name));
        }
    }

    std::cout << "[INFO] Completed FBX model conversion" << std::endl;

    return std::make_shared<bvh_node>(model_output, rnd, name);
}


scene::cameraConfig fbx_mesh_loader::convert_camera_from_file(fbx_mesh_data& data, double aspectRatio)
{
    scene::cameraConfig cam_config{};

    const ofbx::IScene* scene = data.scene;
    const int camera_count = scene->getCameraCount();

    std::cout << "[INFO] Building FBX model (" << camera_count << " cameras found)" << std::endl;

    for (int cam_idx = 0; cam_idx < camera_count; ++cam_idx)
    {
        const ofbx::Camera* cam = scene->getCamera(cam_idx);
        if (cam)
        {
            if (cam->getProjectionType() == ofbx::Camera::ProjectionType::ORTHOGRAPHIC)
            {
                // Orthographic camera
                cam_config.orthoHeight = 2;
                cam_config.fov = 0;
                cam_config.isOrthographic = true;
            }
            else
            {
                // Perspective camera
                cam_config.fov = cam->getFocusDistance(); // fov in 3ds max free camera
                cam_config.orthoHeight = 0;
                cam_config.isOrthographic = false;
            }

            cam_config.aspectRatio = aspectRatio;

            // Extract the camera's local transformation matrix
            const ofbx::DMatrix cam_transform = cam->getLocalTransform();
            matrix4x4 local_transform;

            // Convert ofbx::DMatrix to your matrix4x4 structure
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    local_transform.m[i][j] = cam_transform.m[i * 4 + j];

            // Extract position (lookFrom)from the local transform matrix (translation components)
            vector3 look_from(
                local_transform.m[0][3], // X position
                local_transform.m[1][3], // Y position
                local_transform.m[2][3]  // Z position
            );

            auto zz = cam->getInterestPosition();
            //auto zz2 = convertToMaxSystem(vector3(zz.x, zz.y, zz.z));

            // Interest position (lookAt) - provided by OpenFBX
            vector3 look_at(zz.x, zz.y, zz.z);

            // Default up axis
            vector4 up_axis = extractUpAxis(cam->getLocalTransform());
            up_axis = local_transform * up_axis;

            // Assign the extracted values to the camera config
            cam_config.lookFrom = look_from;
            cam_config.lookAt = vector3(look_at.x, look_at.y, look_at.z);
            cam_config.upAxis = vector3(up_axis.x, up_axis.y, up_axis.z);

            // Additional camera properties
            cam_config.aperture = 0.0;
            cam_config.focus = cam->getFocalLength(); // lens in 3ds max free camera
            cam_config.openingTime = 0.0;

            std::cout << "[INFO] Camera " << cam_idx
                << " - LookFrom: (" << cam_config.lookFrom.x << ", " << cam_config.lookFrom.y << ", " << cam_config.lookFrom.z << ")"
                << " LookAt: (" << cam_config.lookAt.x << ", " << cam_config.lookAt.y << ", " << cam_config.lookAt.z << ")"
                << " UpAxis: (" << cam_config.upAxis.x << ", " << cam_config.upAxis.y << ", " << cam_config.upAxis.z << ")"
                << std::endl;
        }
    }

    return cam_config;
}

vector4 fbx_mesh_loader::extractUpAxis(const ofbx::DMatrix& cam_transform)
{
    // The up axis is the second column of the local transform matrix
    return vector4(
        cam_transform.m[1 * 4 + 0], // X component of up axis
        cam_transform.m[1 * 4 + 1], // Y component of up axis
        cam_transform.m[1 * 4 + 2], // Z component of up axis
        0.0
    );
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
matrix4x4 fbx_mesh_loader::getLocalTransform(const ofbx::Mesh* mesh)
{
    auto scale = mesh->getLocalScaling();
    auto rotation = mesh->getLocalRotation();
    auto translation = mesh->getLocalTranslation();

    matrix4x4 transform;

    // Create scaling matrix
    matrix4x4 S = matrix4x4::scaling(scale.x, scale.y, scale.z);

    // Create rotation matrices (assuming rotation in XYZ order)
    matrix4x4 Rx = matrix4x4::rotationX(rotation.x);
    matrix4x4 Ry = matrix4x4::rotationY(rotation.y);
    matrix4x4 Rz = matrix4x4::rotationZ(rotation.z);

    matrix4x4 R = Rz * Ry * Rx; // Combine rotations

    // Create translation matrix
    matrix4x4 T = matrix4x4::translation(translation.x, translation.y, translation.z);

    // Combine transformations: T * R * S
    transform = T * R * S;

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