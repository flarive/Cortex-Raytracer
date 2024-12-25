#pragma once

#include "../primitives/triangle.h"
#include "../textures/image_texture.h"
#include "../textures/displacement_texture.h"
#include "../materials/phong_material.h"
#include "../misc/bvh_node.h"
#include "../misc/singleton.h"

#include "../cameras/camera.h"
#include "../lights/light.h"

//#define OFBX_DOUBLE_PRECISION
#include "ofbx.h"

#include "../utilities/matrix4x4.h"

#include <memory>
#include <string>

// Helper constant to convert radians to degrees
const double RAD_TO_DEG = 180.0 / M_PI;


class fbx_mesh_loader
{
public:
    fbx_mesh_loader();

    typedef struct
    {
        const ofbx::IScene* scene;
    } fbx_mesh_data;

    typedef struct
    {
        float width;
        float height;
    } sensor_dimensions;

    static bool load_model_from_file(const std::string& filepath, fbx_mesh_data& data);

    static std::shared_ptr<hittable> get_meshes(fbx_mesh_data& data, randomizer& rnd, std::string name = "");
    static std::vector<std::shared_ptr<camera>> get_cameras(fbx_mesh_data& data, double aspectRatio, short int index = 0);
    static std::vector<std::shared_ptr<light>> get_lights(fbx_mesh_data& data, short int index = -1);


private:

    static void computeTangentBasis(std::array<vector3, 3>& vertices, std::array<vector2, 3>& uvs, std::array<vector3, 3>& normals, std::array<vector3, 3>& tangents, std::array<vector3, 3>& bitangents);

    static matrix4x4 getGlobalTransform(const ofbx::Mesh* mesh);

    static vector3 extractUpAxis(const ofbx::DMatrix& cam_transform);

    static std::shared_ptr<phong_material> extractMeshMaterials(const ofbx::Mesh* mesh);

    static vector3 convertToMaxSystem(const vector3& openfbxVector);

    static vector3 convertFromMaxSystem(const vector3& maxSystemVector);

    static double vectorLength(const ofbx::DVec3& vec);

    static double getVerticalFOV(const ofbx::Object* camera, float sensorHeight);
    static sensor_dimensions calculateSensorDimensions(float diagonal, float aspectRatio);

    // Function to decompose a DMatrix into translation, rotation, and scale
    static void decomposeDMatrix(const ofbx::DMatrix& matrix, ofbx::DVec3& translation, ofbx::DVec3& rotation, ofbx::DVec3& scale);

    static void get_metadata(const ofbx::IScene* scene);
};