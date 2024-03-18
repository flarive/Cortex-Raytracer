#ifndef PHONG_H
#define PHONG_H

#include "../constants.h"
#include "../misc/ray.h"
#include "../misc/color.h"
#include "../textures/texture.h"
#include "../primitives/hittable.h"
#include "../materials/material.h"

/// <summary>
/// Phong material
//ici ton material n'a que des informations sur la couleur 
//pas sur ses capacité de diffusion / reflexion
//https://github.com/kdridi/raytracer/blob/main/raytracer/src/material.cpp
//Color Material::lighting(const Shape & shape, const PointLight & light, const Point & point, const Vector & eyev, const Vector & normalv, bool inShadow) const
//return ambient + diffuse + specular;
//pour calculer la lumière il faut mixer ces trois composantes
/// </summary>
class phong : public material
{
public:
    phong(const color& color, double ambient, double diffuse, double specular, double shininess, double reflective, double transparency) :
        m_color(color::White()), m_ambient(0.1), m_diffuse(0.9), m_specular(0.9), m_shininess(200.0), reflective(0.0), transparency(0.0), refractiveIndex(1.0)
    {
    }




    bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override
    {
        // ???????????????????????????

        return true;
    }

    color& getColor()
    {
        return m_color;
    }

    double& getAmbient()
    {
        return m_ambient;
    }

    double& getDiffuse()
    {
        return m_diffuse;
    }

    double& getSpecular()
    {
        return m_specular;
    }

    double& getShininess()
    {
        return m_shininess;
    }

public:
    color& getColor();
    double& getAmbient();
    double& getDiffuse();
    double& getSpecular();
    double& getShininess();

    //APattern* pattern; // ?????????
    double reflective;
    double transparency;
    double refractiveIndex;

private:
    color m_color;
    double m_ambient;
    double m_diffuse;
    double m_specular;
    double m_shininess;
};

#endif