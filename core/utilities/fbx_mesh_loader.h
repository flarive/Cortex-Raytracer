#pragma once

#include "../primitives/triangle.h"
#include "../textures/image_texture.h"
#include "../textures/displacement_texture.h"
#include "../materials/phong_material.h"
#include "../misc/bvh_node.h"
#include "../misc/material_shader_model.h"

#include "../cameras/camera.h"
#include "../lights/light.h"

//#define OFBX_DOUBLE_PRECISION
#include "openfbx/ofbx.h"

#include "../utilities/matrix4x4.h"

#include <memory>
#include <string>
#include <map>

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
        double width;
        double height;
        double diagonal;
    } sensor_dimensions;

    enum class fbx_app
    {
        Undefined = 0,
        Max = 1,
        Blender = 2,
        Other = 3
    };

    static bool load_model_from_file(const std::string& filepath, fbx_mesh_data& data);

    static std::shared_ptr<hittable> get_meshes(fbx_mesh_data& data, randomizer& rnd, const std::map<std::string, std::shared_ptr<material>>& scene_materials, const std::map<std::string, std::shared_ptr<texture>>& scene_textures, bool use_fbx_materials, bool use_fbx_textures, std::string name = "");
    static std::vector<std::shared_ptr<camera>> get_cameras(fbx_mesh_data& data, double aspectRatio, short int index = 0);
    static std::vector<std::shared_ptr<light>> get_lights(fbx_mesh_data& data, short int index = -1);


private:

    static void computeTangentBasis(std::array<vector3, 3>& vertices, std::array<vector2, 3>& uvs, std::array<vector3, 3>& normals, std::array<vector3, 3>& tangents, std::array<vector3, 3>& bitangents);

    static matrix4x4 convertMatrix(const ofbx::DMatrix matrix);

    static vector3 extractUpAxis(const ofbx::DMatrix& cam_transform);

    static std::shared_ptr<material> get_mesh_materials(const ofbx::Mesh* mesh, fbx_mesh_data& data, bool use_fbx_materials, bool use_fbx_textures, const std::map<std::string, std::shared_ptr<material>>& scene_materials, const std::map<std::string, std::shared_ptr<texture>>& scene_textures);

    static std::shared_ptr<texture> get_texture(const ofbx::Material* mat, ofbx::Texture::TextureType textureKind, const std::map<std::string, std::shared_ptr<texture>>& scene_textures, bool use_fbx_textures, double amount);

    static vector3 convertToMaxSystem(const vector3& openfbxVector);
    static vector3 convertFromMaxSystem(const vector3& maxSystemVector);

    static vector3 convertToBlenderSystem(const vector3& fbxVector);
    static vector3 convertFromBlenderSystem(const vector3& blenderVector);

    static double vectorLength(const ofbx::DVec3& vec);

    static double calculateVerticalFOV(double focalLength, double sensorHeight);
    static sensor_dimensions calculateSensorDimensions(double focalLength, double aspectRatio, double sensorWidth = 36.0);

    static double calculateOrthoHeight(const ofbx::Camera* camera, double aspectRatio);

    static void decomposeDMatrix(const ofbx::DMatrix& matrix, ofbx::DVec3& translation, ofbx::DVec3& rotation, ofbx::DVec3& scale);
    static void decomposeDMatrix2(const ofbx::DMatrix& matrix, ofbx::DVec3& translation, ofbx::DVec3& rotation, ofbx::DVec3& scale, ofbx::RotationOrder order);

    static vector3 transform_vector(const ofbx::DMatrix& matrix, const vector3& vec);

    static void apply_transformation_to_omni(const ofbx::DMatrix& matrix, vector3& pos);

    static void apply_transformation_to_directional(const ofbx::DMatrix& matrix, vector3& pos, vector3& u, vector3& v);

    static fbx_mesh_loader::fbx_app get_fbx_appname(const ofbx::IScene* scene);

    static color to_color(ofbx::Color rgb);
    static double scaleValue(double input, double sourceMin, double sourceMax, double targetMin, double targetMax);
    static double scaleLightIntensity(fbx_mesh_data& data, double input);
    static double scaleMaterialShininess(fbx_mesh_data& data, double input);

    static double getSensorWidth(fbx_mesh_data& data);

    static vector3 toVector3(ofbx::DVec3 dv);

    static std::string toString(ofbx::DataView data);
};