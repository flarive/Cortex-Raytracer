#pragma once

#include "../primitives/hittable_list.h"
#include "../materials/material.h"
#include "../primitives/hittable.h"
#include "../utilities/types.h"
#include "../utilities/uvmapping.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../cameras/perspective_camera.h"
#include <string>
#include <map>


class scene_builder
{
    public:
        scene_builder();
        ~scene_builder() = default;

        [[nodiscard]] perspective_camera getCamera() const;
        [[nodiscard]] hittable_list getSceneObjects() const;
        [[nodiscard]] scene::imageConfig getImageConfig() const;
        [[nodiscard]] scene::cameraConfig getCameraConfig() const;

        // Image
        scene_builder& setImageConfig(const  scene::imageConfig& config);
        scene_builder& setImageBackgroundConfig(const color& rgb, const std::string& filepath, bool is_skybox);
        scene_builder& imageSize(int width, int height);
        scene_builder& imageWidth(int width);
        scene_builder& imageHeight(int height);
        scene_builder& imageWidthWithAspectRatio(double aspectRatio);
        scene_builder& imageHeightWithAspectRatio(double aspectRatio);
        scene_builder& imageDepth(int depth);
        scene_builder& imageSamplesPerPixel(int samplesPerPixel);
        scene_builder& imageOutputFilePath(std::string filepath);

        // Camera
        scene_builder& setCameraConfig(const  scene::cameraConfig& config);
        scene_builder& cameraAspectRatio(std::string aspectRatio);
        scene_builder& cameraOpeningTime(double time);
        scene_builder& cameraLookFrom(point3 point);
        scene_builder& cameraLookAt(point3 lookAt);
        scene_builder& cameraUpAxis(point3 vUp);
        scene_builder& cameraAperture(double aperture);
        scene_builder& cameraFocus(double focus);
        scene_builder& cameraFOV(double fov);
        scene_builder& cameraIsOrthographic(bool orthographic);
        scene_builder& cameraOrthoHeight(double height);

        // Textures
        scene_builder& addSolidColorTexture(const std::string& textureName, const color& rgb);
        scene_builder& addCheckerTexture(const std::string& textureName, double scale, const color& oddColor, const color& evenColor);
        scene_builder& addCheckerTexture(const std::string& textureName, double scale, const std::string& oddTextureName, const std::string& evenTextureName);
        scene_builder& addNoiseTexture(const std::string &textureName, double scale = 1.0);
        scene_builder& addMarbleTexture(const std::string& textureName, double scale = 1.0);
        scene_builder& addImageTexture(const std::string& textureName, const std::string &filepath);
        scene_builder& addNormalTexture(const std::string& textureName, const std::string& filepath, double strength);
        scene_builder& addGradientColorTexture(const std::string& textureName, const color& color1, const color& color2, bool aligned_v, bool hsv);
        scene_builder& addBumpTexture(const std::string& textureName, const std::string& filepath, double strength);
        scene_builder& addDisplacementTexture(const std::string& textureName, const std::string& filepath, double strength);
        scene_builder& addAlphaTexture(const std::string& textureName, const std::string& filepath, bool double_sided);
        scene_builder& addEmissiveTexture(const std::string& textureName, const std::string& filepath, double strength);

        // Materials
        scene_builder& addGlassMaterial(const std::string& materialName, double refraction);
        scene_builder& addGlassMaterial(const std::string& materialName, double refraction, const std::string& textureName);
        scene_builder& addGlassMaterial(const std::string& materialName, double refraction, const color& rgb);
        scene_builder& addLambertianMaterial(const std::string& materialName, const color& rgb);
        scene_builder& addLambertianMaterial(const std::string& materialName, const std::string& textureName);
        scene_builder& addPhongMaterial(const std::string& materialName, const std::string& diffuseTextureName, const std::string& specularTextureName, std::string& normalTextureName, const std::string& bumpTextureName, std::string& displaceTextureName, std::string& alphaTextureName, std::string& emissiveTextureName, const color& ambient, double shininess);
        scene_builder& addOrenNayarMaterial(const std::string& materialName, const color& rgb, double albedo_temp, double roughness);
        scene_builder& addOrenNayarMaterial(const std::string& materialName, const std::string& textureName, double albedo_temp, double roughness);
        scene_builder& addIsotropicMaterial(const std::string& materialName, const color& rgb);
        scene_builder& addIsotropicMaterial(const std::string& materialName, const std::string& textureName);
        scene_builder& addAnisotropicMaterial(const std::string& materialName, double nu, double nv, const std::string& diffuseTextureName, const std::string& specularTextureName, const std::string& exponentTextureName);
        scene_builder& addAnisotropicMaterial(const std::string& materialName, double nu, double nv, const color& rgb);
        scene_builder& addMetalMaterial(const std::string& materialName, const color& rgb, double fuzziness);
        scene_builder& addMetalMaterial(const std::string& materialName, const std::string& textureName, double fuzziness, double specular_intensity, const color& metal_tint, double anisotropy, double fresnel_factor, double heat);
        scene_builder& addEmissiveMaterial(const std::string& materialName, const std::string& emissiveTextureName, double intensity);
        scene_builder& addEmissiveMaterial(const std::string& materialName, const color& rgb, double intensity);

        // Lights
        scene_builder& addDirectionalLight(const point3& pos, const vector3& u, const vector3& v, double intensity, const color& rgb, bool invisible, std::string name);
        scene_builder& addOmniDirectionalLight(const point3& pos, double radius, double intensity, const color& rgb, bool invisible, std::string name);
        scene_builder& addSpotLight(const point3& pos, const vector3& dir, double cosTotalWidth, double cosFalloffStart, double intensity, double radius, const color& rgb, bool invisible, std::string name);
        //scene_builder& addDirectionalLightMaterial(const std::string& materialName, const std::string& textureName);
        //scene_builder& setAmbianceLight(color rgb);

        // Primitives
        scene_builder& addObject(const std::shared_ptr<hittable>& obj);
        scene_builder& addSphere(std::string name, point3 pos, double radius, const std::string& materialName, const uvmapping& uv, const std::string& group);
        scene_builder& addQuad(std::string name, const point3& position, const vector3& u, const vector3& v, const std::string& materialName, const uvmapping& uv, const std::string& group);
        scene_builder& addPlane(std::string name, const point3& p0, const point3& p1, const std::string& materialName, const uvmapping& uv, const std::string& group);
        scene_builder& addBox(std::string name, point3 p0, point3 p1, const std::string& materialName, const uvmapping& uv, const std::string& group = "");
        scene_builder& addCylinder(std::string name, point3 pos, double radius, double height, const std::string& materialName, const uvmapping& uv, const std::string& group);
        scene_builder& addCone(std::string name, point3 pos, double radius, double height, const std::string& materialName, const uvmapping& uv, const std::string& group);
        scene_builder& addDisk(std::string name, point3 pos, double radius, double height, const std::string& materialName, const uvmapping& uv, const std::string& group);
        scene_builder& addTorus(std::string name, point3 pos, double major_radius, double minor_radius, const std::string& materialName, const uvmapping& uv, const std::string& group);
        scene_builder& addVolume(std::string name, std::string boundaryObjectName, double density, const std::string& textureName, const std::string& group);
        scene_builder& addVolume(std::string name, std::string boundaryObjectName, double density, const color& rgb, const std::string& group);

        // Meshes
        scene_builder& addObjMesh(std::string name, point3 pos, const std::string& filepath, const std::string& materialName, bool use_mtl, bool use_smoothing, const std::string& group, randomizer& rnd);
        scene_builder& addFbxMesh(std::string name, point3 pos, const std::string& filepath, bool use_fbx_cameras, bool use_fbx_lights, bool use_fbx_materials, bool use_fbx_textures, const std::string& group, randomizer& rnd);

        // Groups
        scene_builder& addGroup(std::string name, bool& found, randomizer& rnd);

        // Transform utils
        scene_builder& translate(const vector3& vector, std::string name = "");
        scene_builder& rotate(const vector3& vector, std::string name = "");
        scene_builder& scale(const vector3& vector, std::string name = "");

    private:
        scene::imageConfig m_imageConfig{};
        scene::cameraConfig m_cameraConfig{};

		std::map<std::string, std::shared_ptr<texture>> m_textures{};
		std::map<std::string, std::shared_ptr<material>> m_materials{};
        std::map<std::string, std::shared_ptr<hittable_list>> m_groups{};
		hittable_list m_objects{};

        std::shared_ptr<material> fetchMaterial(const std::string& name);
        std::shared_ptr<texture> fetchTexture(const std::string& name);
};