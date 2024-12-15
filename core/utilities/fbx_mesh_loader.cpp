#include "fbx_mesh_loader.h"

#include "../materials/lambertian_material.h"
#include "../textures/image_texture.h"

#include "../cameras/perspective_camera.h"
#include "../cameras/orthographic_camera.h"

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
	std::shared_ptr<texture> tex = std::make_shared<image_texture>("../../data/models/giantbug_diffuse.jpg");
	std::shared_ptr<material> model_material = std::make_shared<lambertian_material>(tex);


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

		hittable_list shape_triangles;

		// each ofbx::Mesh can have several materials == partitions
		for (int partition_idx = 0; partition_idx < geom.getPartitionCount(); ++partition_idx)
		{
			//std::cout << "[INFO] obj " << mesh_idx << " / grp " << partition_idx << std::endl;
			const ofbx::GeometryPartition& partition = geom.getPartition(partition_idx);

			// partitions most likely have several polygons, they are not triangles necessarily, use ofbx::triangulate if you want triangles
			for (int polygon_idx = 0; polygon_idx < partition.polygon_count; ++polygon_idx)
			{
				const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygon_idx];

				if (polygon.vertex_count > 3)
				{
					// Prepare a buffer for the triangulated indices
					int tri_indices[16]; // Enough space for up to 5 triangles (5 * 3 = 15)
					assert(polygon.vertex_count <= 16); // Ensure buffer size is sufficient

					// Perform triangulation
					int tri_count = ofbx::triangulate(geom, polygon, tri_indices);
					assert(tri_count >= 1); // Ensure the polygon was successfully triangulated

					// Process each triangle
					for (int tri = 0; tri < tri_count; ++tri)
					{
						std::array<vector3, 3> tri_v{};
						std::array<vector3, 3> tri_vn{};
						std::array<vector2, 3> tri_uv{};

						for (int v = 0; v < 3; ++v)
						{
							// Get vertex index from the triangulated indices
							int vertex_index = tri_indices[tri * 3 + v];

							// Retrieve vertex position
							ofbx::Vec3 pos = positions.get(vertex_index);
							tri_v[v] = vector3(pos.x, pos.y, pos.z);

							// Retrieve normals if available
							if (normals.values)
							{
								ofbx::Vec3 normal = normals.get(vertex_index);
								tri_vn[v] = vector3(normal.x, normal.y, normal.z);
							}

							// Retrieve UVs if available
							if (uvs.values)
							{
								ofbx::Vec2 uv = uvs.get(vertex_index);
								tri_uv[v] = vector2(uv.x, uv.y);
							}
						}

						// Calculate tangent and bitangent for normal texture
						std::array<vector3, 3> tri_tan{}; // triangle tangents
						std::array<vector3, 3> tri_bitan{}; // triangle bitangents
						computeTangentBasis(tri_v, tri_uv, tri_vn, tri_tan, tri_bitan);

						std::shared_ptr<material> tri_mat = model_material;


						shape_triangles.add(make_shared<triangle>(
							mesh_idx,
							partition_idx,
							tri_v[0], tri_v[1], tri_v[2],
							tri_vn[0], tri_vn[1], tri_vn[2],
							tri_uv[0], tri_uv[1], tri_uv[2],
							tri_tan[0], tri_tan[1], tri_tan[2],
							tri_bitan[0], tri_bitan[1], tri_bitan[2],
							shade_smooth, tri_mat));
					}
				}
				else
				{
					std::array<vector3, 3> tri_v{};
					std::array<vector3, 3> tri_vn{};
					std::array<vector2, 3> tri_uv{};

					int loop = 0;

					// Loop over vertices in the face.
					for (int v = polygon.from_vertex; v < polygon.from_vertex + polygon.vertex_count; ++v)
					{
						ofbx::Vec3 vertice = positions.get(v);
						tri_v[loop] = vector3(vertice.x, vertice.y, vertice.z);

						if (normals.values)
						{
							ofbx::Vec3 normal = normals.get(v);
							tri_vn[loop] = vector3(normal.x, normal.y, normal.z);
						}

						// Optionally, retrieve UV coordinates if they are available
						if (uvs.values)
						{
							ofbx::Vec2 uv = uvs.get(v);
							tri_uv[loop] = vector2(uv.x, uv.y);
						}

						loop++;
					}

					// Calculate tangent and bitangent for normal texture
					std::array<vector3, 3> tri_tan{}; // triangle tangents
					std::array<vector3, 3> tri_bitan{}; // triangle bitangents
					computeTangentBasis(tri_v, tri_uv, tri_vn, tri_tan, tri_bitan);

					std::shared_ptr<material> tri_mat = model_material;


					shape_triangles.add(make_shared<triangle>(
						mesh_idx,
						partition_idx,
						tri_v[0], tri_v[1], tri_v[2],
						tri_vn[0], tri_vn[1], tri_vn[2],
						tri_uv[0], tri_uv[1], tri_uv[2],
						tri_tan[0], tri_tan[1], tri_tan[2],
						tri_bitan[0], tri_bitan[1], tri_bitan[2],
						shade_smooth, tri_mat));
				}
			}

			std::cout << "[INFO] Parsing obj file (object name " << mesh->name << " / " << partition.triangles_count << " tris / " << partition.polygon_count << " polys)" << std::endl;

			// group all object triangles in a bvh node
			//model_output.add(std::make_shared<bvh_node>(shape_triangles, 0, 1));
			model_output.add(std::make_shared<bvh_node>(shape_triangles, rnd, name));
		}
    }

    std::cout << "[INFO] Completed FBX model conversion" << std::endl;

    return std::make_shared<bvh_node>(model_output, rnd, name);
}

std::shared_ptr<camera> fbx_mesh_loader::convert_camera_from_file(fbx_mesh_data& data)
{
	std::shared_ptr<camera> cam2;
	
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
				cam2 = std::make_shared<orthographic_camera>();
				cam2->ortho_height = 2;
				cam2->is_orthographic = true;
			}
			else
			{
				cam2 = std::make_shared<perspective_camera>();
				cam2->vfov = cam->getFocalLength(); // cam->getFocusDistance(); ??????
				cam2->is_orthographic = false;
			}

			cam2->aspect_ratio = 1.77777;
			cam2->image_width = 512;
			cam2->samples_per_pixel = 10; // denoiser quality
			cam2->max_depth = 10; // max nbr of bounces a ray can do
			cam2->background_color = color(0.70, 0.80, 1.00);
			cam2->lookfrom = vector3(cam->getInterestPosition().x, cam->getInterestPosition().y, cam->getInterestPosition().z);
			//cam2->lookat = cameraCfg.lookAt;
			cam2->vup = vector3(0.0, 1.0, 0.0);
		}
	}
	
	
	return cam2;
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