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
        matrix4x4 transform = getGlobalTransform(mesh);
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

                        // Transform vertex positions
                        //ofbx::Vec3 pos = positions.get(vertex_index);
                        //vector4 transformed_pos = transform * vector4(pos.x, pos.y, pos.z, 1.0);
                        //tri_v[v] = convertFromMaxSystem(vector3(transformed_pos.x, transformed_pos.y, transformed_pos.z));

                        //if (normals.values)
                        //{
                        //    ofbx::Vec3 normal = normals.get(vertex_index);
                        //    vector4 transformed_normal = normal_transform * vector4(normal.x, normal.y, normal.z, 0.0);
                        //    tri_vn[v] = glm::normalize(convertFromMaxSystem(vector3(transformed_normal.x, transformed_normal.y, transformed_normal.z)));
                        //}

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
            const ofbx::DMatrix cam_transform = cam->getGlobalTransform();

            ofbx::DVec3 translation, rotation, scale;
            decomposeDMatrix(cam_transform, translation, rotation, scale);


            // Extract position (lookFrom)
            vector3 look_from(translation.x, translation.y, translation.z);

            // Interest position (lookAt) - provided by OpenFBX
            auto zz = cam->getInterestPosition();
            vector3 look_at = vector3(zz.x, zz.y, zz.z);

            // Default up axis
            vector3 up_axis = extractUpAxis(cam->getLocalTransform());

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

vector3 fbx_mesh_loader::extractUpAxis(const ofbx::DMatrix& cam_transform)
{
    // The up axis is the second column of the local transform matrix
    return vector3(
        cam_transform.m[1 * 4 + 0], // X component of up axis
        cam_transform.m[1 * 4 + 1], // Y component of up axis
        cam_transform.m[1 * 4 + 2] // Z component of up axis
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
matrix4x4 fbx_mesh_loader::getGlobalTransform(const ofbx::Mesh* mesh)
{
    ofbx::DMatrix zzz = mesh->getGlobalTransform();

    matrix4x4 transform;
    transform.m[0][0] = zzz.m[0];
    transform.m[0][1] = zzz.m[1];
    transform.m[0][2] = zzz.m[2];
    transform.m[0][3] = zzz.m[3];

    transform.m[1][0] = zzz.m[4];
    transform.m[1][1] = zzz.m[5];
    transform.m[1][2] = zzz.m[6];
    transform.m[1][3] = zzz.m[7];

    transform.m[2][0] = zzz.m[8];
    transform.m[2][1] = zzz.m[9];
    transform.m[2][2] = zzz.m[10];
    transform.m[2][3] = zzz.m[11];

    transform.m[3][0] = zzz.m[12];
    transform.m[3][1] = zzz.m[13];
    transform.m[3][2] = zzz.m[14];
    transform.m[3][3] = zzz.m[15];

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
    translation.x = matrix.m[3];
    translation.y = matrix.m[7];
    translation.z = matrix.m[11];

    // Extract scale (length of basis vectors)
    ofbx::DVec3 basisX = { matrix.m[0], matrix.m[4], matrix.m[8] };
    ofbx::DVec3 basisY = { matrix.m[1], matrix.m[5], matrix.m[9] };
    ofbx::DVec3 basisZ = { matrix.m[2], matrix.m[6], matrix.m[10] };

    scale.x = vectorLength(basisX);
    scale.y = vectorLength(basisY);
    scale.z = vectorLength(basisZ);

    // Remove scale from the rotation part
    ofbx::DMatrix rotationMatrix = matrix;
    rotationMatrix.m[0] /= scale.x;
    rotationMatrix.m[4] /= scale.x;
    rotationMatrix.m[8] /= scale.x;

    rotationMatrix.m[1] /= scale.y;
    rotationMatrix.m[5] /= scale.y;
    rotationMatrix.m[9] /= scale.y;

    rotationMatrix.m[2] /= scale.z;
    rotationMatrix.m[6] /= scale.z;
    rotationMatrix.m[10] /= scale.z;

    // Extract rotation as Euler angles (Y-Z-X order)
    rotation.y = std::atan2(rotationMatrix.m[8], rotationMatrix.m[10]); // Yaw
    rotation.x = -std::asin(rotationMatrix.m[9]);                      // Pitch
    rotation.z = std::atan2(rotationMatrix.m[1], rotationMatrix.m[5]); // Roll
}
