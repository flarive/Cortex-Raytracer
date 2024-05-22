//#pragma once
//
//#include "../misc/ray.h"
//#include "../misc/color.h"
//#include "../primitives/hittable.h"
//#include "../materials/material.h"
//#include "../lights/light.h"
//#include "../primitives/hittable_list.h"
//#include "../misc/scatter_record.h"
//#include "../textures/solid_color_texture.h"
//#include "../utilities/randomizer.h"
//
///// <summary>
///// Phong material
///// </summary>
//class phong : public material
//{
//public:
//	phong();
//	phong(const color& _color);
//	phong(const color& _color, double _ambient, double _diffuse, double _specular, double _shininess);
//	phong(const color& _color, double _ambient, double _diffuse, double _specular, double _shininess, double _transparency, double _refraction_index);
//
//	phong(std::shared_ptr<texture> _albedo);
//	phong(std::shared_ptr<texture> _albedo, double _ambient, double _diffuse, double _specular, double _shininess);
//	phong(std::shared_ptr<texture> _albedo, double _ambient, double _diffuse, double _specular, double _shininess, double _transparency, double _refraction_index);
//
//	phong(std::shared_ptr<texture> _albedo, std::shared_ptr<texture> _normal, double _ambient, double _diffuse, double _specular, double _shininess, double _transparency, double _refraction_index);
//
//	bool scatter(const ray& r_in, const hittable_list& lights, const hit_record& rec, scatter_record& srec, randomizer& random) const override;
//	double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override;
//   
//
//public:
//	double& getAmbient();
//	double& getDiffuse();
//	double& getSpecular();
//	double& getShininess();
//
//
//private:
//	double m_ambient = 0.0;
//    double m_diffuse = 0.0;
//    double m_specular = 0.0;
//    double m_shininess = 0.0;
//
//	static bool refract(const vector3& incident, const vector3& normal, double refractive_index, vector3& refracted);
//};