#pragma once

#include "../primitives/hittable_list.h"
#include "../materials/material.h"
#include "../primitives/hittable.h"
#include "types.h"
#include "uvmapping.h"
#include "../textures/texture.h"
#include "../cameras/camera.h"
#include "../cameras/target_camera.h"
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

class SceneBuilder
{
    public:
        SceneBuilder();
        ~SceneBuilder() = default;

        [[nodiscard]] target_camera getCamera() const;
        [[nodiscard]] hittable_list getScene() const;
        [[nodiscard]] imageConfig getImageConfig() const;
        [[nodiscard]] cameraConfig getCameraConfig() const;
        //[[nodiscard]] std::map<std::string, std::shared_ptr<texture>> getTextures() const;
        //[[nodiscard]] std::map<std::string, std::shared_ptr<material>> getMaterials() const;

        // Image
        SceneBuilder& setImageConfig(const imageConfig& config);
        SceneBuilder& imageSize(int width, int height);
        SceneBuilder& imageWidth(int width);
        SceneBuilder& imageHeight(int height);
        SceneBuilder& imageWidthWithAspectRatio(double aspectRatio);
        SceneBuilder& imageHeightWithAspectRatio(double aspectRatio);
        SceneBuilder& imageDepth(int depth);
        SceneBuilder& imageSamplesPerPixel(int samplesPerPixel);
        SceneBuilder& imageBackgroundColor(color c);

        // Camera
        SceneBuilder& setCameraConfig(const cameraConfig& config);
        SceneBuilder& cameraAspectRatio(double aspectRatio);
        SceneBuilder& cameraOpeningTime(double time);
        SceneBuilder& cameraLookFrom(point3 point);
        SceneBuilder& cameraLookAt(point3 lookAt);
        SceneBuilder& cameraUpAxis(point3 vUp);
        SceneBuilder& cameraAperture(double aperture);
        SceneBuilder& cameraFocus(double focus);
        SceneBuilder& cameraFOV(double fov);

        // Textures
        SceneBuilder& addSolidColorTexture(const std::string& textureName, color rgb);
        SceneBuilder& addCheckerTexture(const std::string& textureName, double scale, color oddColor, color evenColor);
        SceneBuilder& addCheckerTexture(const std::string& textureName, double scale, const std::string& oddTextureName, const std::string& evenTextureName);
        SceneBuilder& addNoiseTexture(const std::string &textureName, double scale = 1.0);
        SceneBuilder& addMarbleTexture(const std::string& textureName, double scale = 1.0);
        SceneBuilder& addImageTexture(const std::string& textureName, const std::string &filepath);
        SceneBuilder& addGradientColorTexture(const std::string& textureName, color color1, color color2, bool aligned_v, bool hsv);

        // Materials
        SceneBuilder& addGlassMaterial(const std::string& materialName, double refraction);
        SceneBuilder& addLambertianMaterial(const std::string& materialName, const color& rgb);
        SceneBuilder& addLambertianMaterial(const std::string& materialName, const std::string& textureName);
        SceneBuilder& addPhongMaterial(const std::string& materialName, const color& rgb, double ambiant, double diffuse, double specular, double shininess, double transparency, double refraction_index);
        SceneBuilder& addPhongMaterial(const std::string& materialName, const std::string& textureName, double ambiant, double diffuse, double specular, double shininess, double transparency, double refraction_index);
        SceneBuilder& addOrenNayarMaterial(const std::string& materialName, const color& rgb, double albedo_temp, double roughness);
        SceneBuilder& addOrenNayarMaterial(const std::string& materialName, const std::string& textureName, double albedo_temp, double roughness);
        SceneBuilder& addIsotropicMaterial(const std::string& materialName, const color& rgb);
        SceneBuilder& addIsotropicMaterial(const std::string& materialName, const std::string& textureName);
        SceneBuilder& addAnisotropicMaterial(const std::string& materialName, const color& rgb, double roughness);
        SceneBuilder& addAnisotropicMaterial(const std::string& materialName, const std::string& textureName, double roughness);
        SceneBuilder& addMetalMaterial(const std::string& materialName, color rgb, double fuzz);
        SceneBuilder& addDielectricMaterial(const std::string& materialName, double index_of_refraction);

        // Lights
        SceneBuilder& addDirectionalLight(const point3& pos, const vector3& u, const vector3& v, double intensity, color rgb, bool invisible, std::string name);
        SceneBuilder& addOmniDirectionalLight(const point3& pos, double radius, double intensity, color rgb, bool invisible, std::string name);
        //SceneBuilder& addDirectionalLightMaterial(const std::string& materialName, const std::string& textureName);
        //SceneBuilder& setAmbianceLight(color rgb);

        // Primitives
        SceneBuilder& addObject(const std::shared_ptr<hittable>& obj);
        SceneBuilder& addSphere(std::string name, point3 pos, double radius, const std::string& material, const uvmapping& uv);
        SceneBuilder& addPlane(std::string name, point3 p0, point3 p1, const std::string& material, const uvmapping& uv);
        SceneBuilder& addBox(std::string name, point3 p0, point3 p1, const std::string& material, const uvmapping& uv);
        SceneBuilder& addCylinder(std::string name, point3 pos, double radius, double height, const std::string& material, const uvmapping& uv);
        SceneBuilder& addCone(std::string name, point3 pos, double radius, double height, const std::string& material, const uvmapping& uv);
        SceneBuilder& addDisk(std::string name, point3 pos, double radius, double height, const std::string& material, const uvmapping& uv);
        SceneBuilder& addTorus(std::string name, point3 pos, double major_radius, double minor_radius, const std::string& material, const uvmapping& uv);

        // Transform utils
        SceneBuilder& translate(const vector3& vector);
        SceneBuilder& rotate(const vector3& vector);
        SceneBuilder& scale(const vector3& vector);

    private:
		imageConfig _imageConfig{};
		cameraConfig _cameraConfig{};

		std::map<std::string, std::shared_ptr<texture>> _textures{};
		std::map<std::string, std::shared_ptr<material>> _materials{};
		hittable_list _objects{};
};