#pragma once

#include "../primitives/hittable_list.h"
#include "../materials/material.h"
#include "../primitives/hittable.h"
#include "../utilities/types.h"
#include "../utilities/uvmapping.h"
#include "../textures/texture.h"
#include "../cameras/camera.h"
#include "../cameras/target_camera.h"
#include "../bvh_node.h"
#include <ostream>
#include <vector>
#include <string>
#include <map>




typedef struct {
    int height;
    int width;
    int depth;
    int spp;
    color backgroundColor;
} imageConfig;

typedef struct {
    double aspectRatio;
    double openingTime;
    point3 lookFrom;
    point3 lookAt;
    point3 upAxis;
    double aperture;
    double focus;
    double fov;
} cameraConfig;

class scene_builder
{
    public:
        scene_builder();
        ~scene_builder() = default;

        [[nodiscard]] target_camera getCamera() const;
        [[nodiscard]] hittable_list getScene() const;
        [[nodiscard]] imageConfig getImageConfig() const;
        [[nodiscard]] cameraConfig getCameraConfig() const;
        //[[nodiscard]] std::map<std::string, std::shared_ptr<texture>> getTextures() const;
        //[[nodiscard]] std::map<std::string, std::shared_ptr<material>> getMaterials() const;

        // Image
        scene_builder& setImageConfig(const imageConfig& config);
        scene_builder& imageSize(int width, int height);
        scene_builder& imageWidth(int width);
        scene_builder& imageHeight(int height);
        scene_builder& imageWidthWithAspectRatio(double aspectRatio);
        scene_builder& imageHeightWithAspectRatio(double aspectRatio);
        scene_builder& imageDepth(int depth);
        scene_builder& imageSamplesPerPixel(int samplesPerPixel);
        scene_builder& imageBackgroundColor(color c);

        // Camera
        scene_builder& setCameraConfig(const cameraConfig& config);
        scene_builder& cameraAspectRatio(std::string aspectRatio);
        scene_builder& cameraOpeningTime(double time);
        scene_builder& cameraLookFrom(point3 point);
        scene_builder& cameraLookAt(point3 lookAt);
        scene_builder& cameraUpAxis(point3 vUp);
        scene_builder& cameraAperture(double aperture);
        scene_builder& cameraFocus(double focus);
        scene_builder& cameraFOV(double fov);

        // Textures
        scene_builder& addSolidColorTexture(const std::string& textureName, color rgb);
        scene_builder& addCheckerTexture(const std::string& textureName, double scale, color oddColor, color evenColor);
        scene_builder& addCheckerTexture(const std::string& textureName, double scale, const std::string& oddTextureName, const std::string& evenTextureName);
        scene_builder& addNoiseTexture(const std::string &textureName, double scale = 1.0);
        scene_builder& addMarbleTexture(const std::string& textureName, double scale = 1.0);
        scene_builder& addImageTexture(const std::string& textureName, const std::string &filepath);
        scene_builder& addNormalTexture(const std::string& textureName, const std::string& filepath, double strength);
        scene_builder& addGradientColorTexture(const std::string& textureName, color color1, color color2, bool aligned_v, bool hsv);
        scene_builder& addBumpTexture(const std::string& textureName, const std::string& filepath, double scale);

        // Materials
        scene_builder& addGlassMaterial(const std::string& materialName, double refraction);
        scene_builder& addLambertianMaterial(const std::string& materialName, const color& rgb);
        scene_builder& addLambertianMaterial(const std::string& materialName, const std::string& textureName);
        //scene_builder& addPhongMaterial(const std::string& materialName, const color& rgb, double ambiant, double diffuse, double specular, double shininess, double transparency, double refraction_index);
        //scene_builder& addPhongMaterial(const std::string& materialName, const std::string& albedoTextureName, const std::string& normalTextureName, double ambiant, double diffuse, double specular, double shininess, double transparency, double refraction_index);

        scene_builder& addPhong2Material(const std::string& materialName, const std::string& diffuseTextureName, const std::string& specularTextureName, std::string& normalTextureName, const std::string& bumpTextureName, const color& ambient, double shininess);

        scene_builder& addOrenNayarMaterial(const std::string& materialName, const color& rgb, double albedo_temp, double roughness);
        scene_builder& addOrenNayarMaterial(const std::string& materialName, const std::string& textureName, double albedo_temp, double roughness);
        scene_builder& addIsotropicMaterial(const std::string& materialName, const color& rgb);
        scene_builder& addIsotropicMaterial(const std::string& materialName, const std::string& textureName);

        scene_builder& addAnisotropicMaterial(const std::string& materialName, double nu, double nv, const std::string& diffuseTextureName, const std::string& specularTextureName, const std::string& exponentTextureName);

        //scene_builder& addAnisotropicMaterial(const std::string& materialName, const std::string& textureName, double roughness);
        scene_builder& addMetalMaterial(const std::string& materialName, color rgb, double fuzz);
        

        // Lights
        scene_builder& addDirectionalLight(const point3& pos, const vector3& u, const vector3& v, double intensity, color rgb, bool invisible, std::string name);
        scene_builder& addOmniDirectionalLight(const point3& pos, double radius, double intensity, color rgb, bool invisible, std::string name);
        //scene_builder& addDirectionalLightMaterial(const std::string& materialName, const std::string& textureName);
        //scene_builder& setAmbianceLight(color rgb);

        // Primitives
        scene_builder& addObject(const std::shared_ptr<hittable>& obj);
        scene_builder& addSphere(std::string name, point3 pos, double radius, const std::string& material, const uvmapping& uv);
        scene_builder& addQuad(std::string name, point3 position, vector3 u, vector3 v, const std::string& material, const uvmapping& uv);
        scene_builder& addPlane(std::string name, point3 p0, point3 p1, const std::string& material, const uvmapping& uv);
        scene_builder& addBox(std::string name, point3 p0, point3 p1, const std::string& material, const uvmapping& uv, const std::string& group = "");
        scene_builder& addCylinder(std::string name, point3 pos, double radius, double height, const std::string& material, const uvmapping& uv);
        scene_builder& addCone(std::string name, point3 pos, double radius, double height, const std::string& material, const uvmapping& uv);
        scene_builder& addDisk(std::string name, point3 pos, double radius, double height, const std::string& material, const uvmapping& uv);
        scene_builder& addTorus(std::string name, point3 pos, double major_radius, double minor_radius, const std::string& material, const uvmapping& uv);
        scene_builder& addVolume(std::string name, std::string boundaryObjectName, double density, const std::string& textureName);
        scene_builder& addVolume(std::string name, std::string boundaryObjectName, double density, const color& rgb);

        // Meshes
        scene_builder& addMesh(std::string name, point3 pos, const std::string& filepath, const std::string& material, bool use_mtl, bool use_smoothing);

        // Groups
        scene_builder& addGroup(std::string name);

        // Transform utils
        scene_builder& translate(const vector3& vector);
        scene_builder& rotate(const vector3& vector);
        scene_builder& scale(const vector3& vector);

    private:
		imageConfig m_imageConfig{};
		cameraConfig m_cameraConfig{};

		std::map<std::string, std::shared_ptr<texture>> m_textures{};
		std::map<std::string, std::shared_ptr<material>> m_materials{};
        std::map<std::string, std::shared_ptr<hittable_list>> m_groups{};
		hittable_list m_objects{};
};