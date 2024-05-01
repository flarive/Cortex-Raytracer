#include "Configuration.h"

#include "../textures/image_texture.h"
#include "../textures/solid_color_texture.h"
#include "../utilities/uvmapping.h"
#include "../misc/transform.h"

#include "iostream"
#include <utility>
#include <filesystem>

Configuration::Configuration(std::string path) : _path(std::move(path))
{
}

SceneBuilder Configuration::loadSceneFromFile()
{
	SceneBuilder builder;

	std::filesystem::path dir(std::filesystem::current_path());
	std::filesystem::path file(this->_path.c_str());
	std::filesystem::path fullexternalProgramPath = dir / file;

	auto fullAbsPath = std::filesystem::absolute(fullexternalProgramPath);

	if (std::filesystem::exists(fullAbsPath))
	{
		try
		{
			this->cfg.readFile(fullAbsPath.string());
		}
		catch (const libconfig::ParseException& e)
		{
			std::clog << "Error occurred ! " << e.what() << std::endl;
			std::clog << "Error : " << e.getError() << std::endl;
			std::clog << "Line : " << e.getLine() << std::endl;
			std::clog << "File : " << e.getFile() << std::endl;
			std::clog << "Press any key to quit..." << std::endl;
			system("pause");
			return builder;
		}
		catch (const std::exception& e)
		{
			std::clog << "Error occurred ! " << e.what() << std::endl;
			std::clog << "Press any key to quit..." << std::endl;
			system("pause");
			return builder;
		}
			

		const libconfig::Setting& root = this->cfg.getRoot();

		if (root.exists("image"))
		{
			const libconfig::Setting& image = root["image"];
			this->loadImageConfig(builder, image);
		}

		if (root.exists("camera"))
		{
			const libconfig::Setting& camera = root["camera"];
			this->loadCameraConfig(builder, camera);
		}

		if (root.exists("lights"))
		{
			const libconfig::Setting& lights = root["lights"];
			this->loadLights(builder, lights);
		}

		if (root.exists("textures"))
		{
			const libconfig::Setting& textures = root["textures"];
			this->loadTextures(builder, textures);
		}

		if (root.exists("materials"))
		{
			const libconfig::Setting& materials = root["materials"];
			this->loadMaterials(builder, materials);
		}

		if (root.exists("primitives"))
		{
			const libconfig::Setting& primitives = root["primitives"];
			this->loadPrimitives(builder, primitives);
		}
	}

	return builder;
}

void Configuration::loadTextures(SceneBuilder& builder, const libconfig::Setting& textures)
{
	if (textures.exists("image"))
	{
		const libconfig::Setting& image = textures["image"];

		for (int i = 0; i < image.getLength(); i++)
		{
			const libconfig::Setting& texture = image[i];
			std::string name = "";
			std::string filepath = "";

			if (texture.exists("name"))
				texture.lookupValue("name", name);
			if (texture.exists("filepath"))
				texture.lookupValue("filepath", filepath);

			if (name.empty())
				throw std::runtime_error("Texture name is empty");

			builder.addImageTexture(name, filepath);
		}
	}
	//if (textures.exists("noise"))
	//{
	//	const libconfig::Setting& noise = textures["noise"];

	//	for (int i = 0; i < noise.getLength(); i++) {
	//		const libconfig::Setting& texture = noise[i];
	//		std::string name = "";
	//		double scale = 1.0;

	//		if (texture.exists("name"))
	//			texture.lookupValue("name", name);
	//		if (texture.exists("scale"))
	//			texture.lookupValue("scale", scale);

	//		if (name.empty())
	//			throw std::runtime_error("Texture name is empty");

	//		//builder.addNoiseTexture(name, scale);
	//	}
	//}
	if (textures.exists("solidColor"))
	{
		const libconfig::Setting& solidColor = textures["solidColor"];

		for (int i = 0; i < solidColor.getLength(); i++) {
			const libconfig::Setting& texture = solidColor[i];
			std::string name = "";
			color color = { 0.0, 0.0, 0.0 };

			if (texture.exists("name"))
				texture.lookupValue("name", name);
			if (texture.exists("color"))
				color = this->getColor(texture["color"]);

			if (name.empty())
				throw std::runtime_error("Texture name is empty");

			builder.addSolidColorTexture(name, color);
		}
	}
	//if (textures.exists("chessboard"))
	//{
	//	if (textures["chessboard"].exists("textured")) {
	//		for (int i = 0; i < textures["chessboard"]["textured"].getLength(); i++) {
	//			const libconfig::Setting& texture =
	//				textures["chessboard"]["textured"][i];
	//			std::string name = "";
	//			std::string lightTextureName = "";
	//			std::string darkTextureName = "";

	//			if (texture.exists("name"))
	//				texture.lookupValue("name", name);
	//			if (texture.exists("lightTexture"))
	//				texture.lookupValue("lightTexture", lightTextureName);
	//			if (texture.exists("darkTexture"))
	//				texture.lookupValue("darkTexture", darkTextureName);

	//			if (name.empty())
	//				throw std::runtime_error("Texture name is empty");
	//			if (lightTextureName.empty())
	//				throw std::runtime_error("Light texture name is empty");
	//			if (darkTextureName.empty())
	//				throw std::runtime_error("Dark texture name is empty");

	//			//builder.addChessBoardTexture(name, lightTextureName, darkTextureName);
	//		}
	//	}
	//	if (textures["chessboard"].exists("colored"))
	//	{
	//		for (int i = 0; i < textures["chessboard"]["colored"].getLength(); i++) {
	//			const libconfig::Setting& texture =
	//				textures["chessboard"]["colored"][i];
	//			std::string name = "";
	//			color lightColor = { 0.0, 0.0, 0.0 };
	//			color darkColor = { 0.0, 0.0, 0.0 };

	//			if (texture.exists("name"))
	//				texture.lookupValue("name", name);
	//			if (texture.exists("lightColor"))
	//				lightColor = this->getRGB(texture["lightColor"]);
	//			if (texture.exists("darkColor"))
	//				darkColor = this->getRGB(texture["darkColor"]);

	//			if (name.empty())
	//				throw std::runtime_error("Texture name is empty");

	//			//builder.addChessBoardTexture(name, lightColor, darkColor);
	//		}
	//	}
	//}
}

void Configuration::loadMaterials(SceneBuilder& builder, const libconfig::Setting& setting)
{
	if (setting.exists("lambertian"))
	{
		for (int i = 0; i < setting["lambertian"].getLength(); i++)
		{
			const libconfig::Setting& material = setting["lambertian"][i];
			std::string name{};
			color rgb{};
			std::string textureName{};

			if (material.exists("name"))
				material.lookupValue("name", name);
			if (material.exists("color"))
				rgb = this->getColor(material["color"]);
			if (material.exists("texture"))
				material.lookupValue("texture", textureName);

			if (name.empty())
				throw std::runtime_error("Material name is empty");

			if (!textureName.empty())
				builder.addLambertianMaterial(name, textureName);
			else
				builder.addLambertianMaterial(name, rgb);
		}
	}

	if (setting.exists("phong"))
	{
		for (int i = 0; i < setting["phong"].getLength(); i++)
		{
			const libconfig::Setting& material = setting["phong"][i];
			std::string name{};
			color rgb{};
			std::string textureName{};
			double ambiant = 0.0;
			double diffuse = 0.0;
			double specular = 0.0;
			double shininess = 0.0;
			double transparency = 0.0;
			double refraction_index = 0.0;

			if (material.exists("name"))
				material.lookupValue("name", name);
			if (material.exists("color"))
				rgb = this->getColor(material["color"]);
			if (material.exists("texture"))
				material.lookupValue("texture", textureName);
			if (material.exists("ambiant"))
				material.lookupValue("ambiant", ambiant);
			if (material.exists("diffuse"))
				material.lookupValue("diffuse", diffuse);
			if (material.exists("specular"))
				material.lookupValue("specular", specular);
			if (material.exists("shininess"))
				material.lookupValue("shininess", shininess);
			if (material.exists("transparency"))
				material.lookupValue("transparency", transparency);
			if (material.exists("refraction_index"))
				material.lookupValue("refraction_index", refraction_index);

			if (name.empty())
				throw std::runtime_error("Material name is empty");

			if (!textureName.empty())
				builder.addPhongMaterial(name, textureName, ambiant, diffuse, specular, shininess, transparency, refraction_index);
			else
				builder.addPhongMaterial(name, rgb, ambiant, diffuse, specular, shininess, transparency, refraction_index);
		}
	}

	if (setting.exists("orennayar"))
	{
		for (int i = 0; i < setting["orennayar"].getLength(); i++)
		{
			const libconfig::Setting& material = setting["orennayar"][i];
			std::string name{};
			color rgb{};
			std::string textureName{};
			double albedo_temp = 0.0;
			double roughness = 0.0;

			if (material.exists("name"))
				material.lookupValue("name", name);
			if (material.exists("color"))
				rgb = this->getColor(material["color"]);
			if (material.exists("texture"))
				material.lookupValue("texture", textureName);
			if (material.exists("albedo_temp"))
				material.lookupValue("albedo_temp", albedo_temp);
			if (material.exists("roughness"))
				material.lookupValue("roughness", roughness);

			if (name.empty())
				throw std::runtime_error("Material name is empty");

			if (!textureName.empty())
				builder.addOrenNayarMaterial(name, textureName, albedo_temp, roughness);
			else
				builder.addOrenNayarMaterial(name, rgb, albedo_temp, roughness);
		}
	}

	if (setting.exists("isotropic"))
	{
		for (int i = 0; i < setting["isotropic"].getLength(); i++)
		{
			const libconfig::Setting& material = setting["isotropic"][i];
			std::string name{};
			color rgb{};
			std::string textureName{};

			if (material.exists("name"))
				material.lookupValue("name", name);
			if (material.exists("color"))
				rgb = this->getColor(material["color"]);
			if (material.exists("texture"))
				material.lookupValue("texture", textureName);

			if (name.empty())
				throw std::runtime_error("Material name is empty");

			if (!textureName.empty())
				builder.addIsotropicMaterial(name, textureName);
			else
				builder.addIsotropicMaterial(name, rgb);
		}
	}

	if (setting.exists("anisotropic"))
	{
		for (int i = 0; i < setting["anisotropic"].getLength(); i++)
		{
			const libconfig::Setting& material = setting["anisotropic"][i];
			std::string name{};
			color rgb{};
			std::string textureName{};
			double roughness = 0.0;

			if (material.exists("name"))
				material.lookupValue("name", name);
			if (material.exists("color"))
				rgb = this->getColor(material["color"]);
			if (material.exists("texture"))
				material.lookupValue("texture", textureName);
			if (material.exists("roughness"))
				material.lookupValue("roughness", roughness);

			if (name.empty())
				throw std::runtime_error("Material name is empty");

			if (!textureName.empty())
				builder.addAnisotropicMaterial(name, textureName, roughness);
			else
				builder.addAnisotropicMaterial(name, rgb, roughness);
		}
	}

	if (setting.exists("glass"))
	{
		for (int i = 0; i < setting["glass"].getLength(); i++)
		{
			const libconfig::Setting& material = setting["glass"][i];
			std::string name = "";
			double refraction = 0.0;

			if (material.exists("name"))
				material.lookupValue("name", name);
			if (material.exists("refraction"))
				material.lookupValue("refraction", refraction);

			if (name.empty())
				throw std::runtime_error("Material name is empty");

			builder.addGlassMaterial(name, refraction);
		}
	}

	if (setting.exists("metal"))
	{
		for (int i = 0; i < setting["metal"].getLength(); i++)
		{
			const libconfig::Setting& material = setting["metal"][i];
			std::string name = "";
			color color = { 0.0, 0.0, 0.0 };
			double fuzziness = 0.0;

			if (material.exists("name"))
				material.lookupValue("name", name);
			if (material.exists("color"))
				color = this->getColor(material["color"]);
			if (material.exists("fuzziness"))
				material.lookupValue("fuzziness", fuzziness);
			if (name.empty())
				throw std::runtime_error("Material name is empty");

			builder.addMetalMaterial(name, color, fuzziness);
		}
	}

	if (setting.exists("dielectric"))
	{
		for (int i = 0; i < setting["dielectric"].getLength(); i++)
		{
			const libconfig::Setting& material = setting["dielectric"][i];
			std::string name = "";
			double index_of_refraction = 0.0;

			if (material.exists("name"))
				material.lookupValue("name", name);
			if (material.exists("index_of_refraction"))
				material.lookupValue("index_of_refraction", index_of_refraction);

			if (name.empty())
				throw std::runtime_error("Material name is empty");

			builder.addDielectricMaterial(name, index_of_refraction);
		}
	}
}

void Configuration::loadLights(SceneBuilder& builder, const libconfig::Setting& lights)
{
	if (lights.exists("quadLights"))
	{
		for (int i = 0; i < lights["quadLights"].getLength(); i++)
		{
			const libconfig::Setting& light = lights["quadLights"][i];
			std::string name{};
			color rgb{};
			point3 position{};
			vector3 u{};
			vector3 v{};
			double intensity{};
			bool invisible = true;

			if (light.exists("name"))
				light.lookupValue("name", name);
			if(light.exists("position"))
				position = this->getVector(light["position"]);
			if (light.exists("u"))
				u = this->getPoint(light["u"]);
			if (light.exists("v"))
				v = this->getPoint(light["v"]);
			if (light.exists("intensity"))
				light.lookupValue("intensity", intensity);
			if (light.exists("color"))
				rgb = this->getColor(light["color"]);
			if (light.exists("invisible"))
				light.lookupValue("invisible", invisible);

			builder.addDirectionalLight(position, u, v, intensity, rgb, invisible, name);
		}
	}

	if (lights.exists("sphereLights"))
	{
		for (int i = 0; i < lights["sphereLights"].getLength(); i++)
		{
			const libconfig::Setting& light = lights["sphereLights"][i];
			std::string name{};
			color rgb{};
			point3 position{};
			double radius = 0.0;
			double intensity = 0.0;
			bool invisible = true;

			if (light.exists("name"))
				light.lookupValue("name", name);
			if (light.exists("position"))
				position = this->getVector(light["position"]);
			if (light.exists("radius"))
				light.lookupValue("radius", radius);
			if (light.exists("intensity"))
				light.lookupValue("intensity", intensity);
			if (light.exists("color"))
				rgb = this->getColor(light["color"]);
			if (light.exists("invisible"))
				light.lookupValue("invisible", invisible);

			builder.addOmniDirectionalLight(position, radius, intensity, rgb, invisible, name);
		}
	}
}

point3 Configuration::getPoint(const libconfig::Setting& setting)
{
	point3 point;
	point.x = 0.0;
	point.y = 0.0;
	point.z = 0.0;

	if (setting.exists("x"))
		setting.lookupValue("x", point.x);
	if (setting.exists("y"))
		setting.lookupValue("y", point.y);
	if (setting.exists("z"))
		setting.lookupValue("z", point.z);

	return point;
}

vector3 Configuration::getVector(const libconfig::Setting& setting)
{
	vector3 vector;
	vector.x = 0.0;
	vector.y = 0.0;
	vector.z = 0.0;

	if (setting.exists("x"))
		setting.lookupValue("x", vector.x);
	if (setting.exists("y"))
		setting.lookupValue("y", vector.y);
	if (setting.exists("z"))
		setting.lookupValue("z", vector.z);

	return vector;
}

uvmapping Configuration::getUVmapping(const libconfig::Setting& setting)
{
	uvmapping uv{};

	double scale_u = 0.0;
	double scale_v = 0.0;
	double offset_u = 0.0;
	double offset_v = 0.0;

	if (setting.exists("scale_u"))
		setting.lookupValue("scale_u", scale_u);
	if (setting.exists("scale_v"))
		setting.lookupValue("scale_v", scale_v);
	if (setting.exists("offset_u"))
		setting.lookupValue("offset_u", offset_u);
	if (setting.exists("offset_v"))
		setting.lookupValue("offset_v", offset_v);

	uv.scale_u(scale_u);
	uv.scale_v(scale_v);
	uv.offset_u(offset_u);
	uv.offset_v(offset_v);

	return uv;
}

rt::transform Configuration::getTransform(const libconfig::Setting& setting)
{
	rt::transform trs;

	if (setting.exists("translate"))
		trs.setTranslate(this->getVector(setting["translate"]));
	if (setting.exists("rotate"))
		trs.setRotate(this->getVector(setting["rotate"]));
	if (setting.exists("scale"))
		trs.setScale(this->getVector(setting["scale"]));

	return trs;
}

void Configuration::loadImageConfig(SceneBuilder& builder, const libconfig::Setting& setting)
{
	if (setting.exists("width"))
		builder.imageWidth(setting["width"]);
	if (setting.exists("height"))
		builder.imageHeight(setting["height"]);
	if (setting.exists("maxDepth"))
		builder.imageDepth(setting["maxDepth"]);
	if (setting.exists("samplesPerPixel"))
		builder.imageSamplesPerPixel(setting["samplesPerPixel"]);
	if (setting.exists("backgroundColor"))
		builder.imageBackgroundColor(getColor(setting["backgroundColor"]));
}

color Configuration::getColor(const libconfig::Setting& setting)
{
	double r = 0.0;
	double g = 0.0;
	double b = 0.0;

	if (setting.exists("r"))
		setting.lookupValue("r", r);
	if (setting.exists("g"))
		setting.lookupValue("g", g);
	if (setting.exists("b"))
		setting.lookupValue("b", b);

	return color(r, g, b);
}



void Configuration::loadCameraConfig(SceneBuilder& builder, const libconfig::Setting& setting)
{
	if (setting.exists("aspectRatio")) {
		builder.cameraAspectRatio(setting["aspectRatio"]);
	}
	if (setting.exists("openingTime")) {
		builder.cameraOpeningTime(setting["openingTime"]);
	}
	if (setting.exists("lookFrom")) {
		builder.cameraLookFrom(this->getPoint(setting["lookFrom"]));
	}
	if (setting.exists("lookAt")) {
		builder.cameraLookAt(this->getPoint(setting["lookAt"]));
	}
	if (setting.exists("upAxis")) {
		builder.cameraUpAxis(this->getPoint(setting["upAxis"]));
	}
	if (setting.exists("aperture")) {
		builder.cameraAperture(setting["aperture"]);
	}
	if (setting.exists("focus")) {
		builder.cameraFocus(setting["focus"]);
	}
	if (setting.exists("fov")) {
		builder.cameraFOV(setting["fov"]);
	}
}

void Configuration::loadPrimitives(SceneBuilder& builder, const libconfig::Setting& setting)
{
	if (setting.exists("spheres"))
	{
		for (int i = 0; i < setting["spheres"].getLength(); i++)
		{
			const libconfig::Setting& primitive = setting["spheres"][i];
			string name = "";
			point3 position{};
			double radius = 0.0;
			std::string materialName = "";
			uvmapping uv = { 1, 1, 0, 0 };

			if (primitive.exists("name"))
				primitive.lookupValue("name", name);
			if (primitive.exists("position"))
				position = this->getPoint(primitive["position"]);
			if (primitive.exists("radius"))
				primitive.lookupValue("radius", radius);
			if (primitive.exists("material"))
				primitive.lookupValue("material", materialName);
			if (primitive.exists("uvmapping"))
				uv = this->getUVmapping(primitive["uvmapping"]);

			if (materialName.empty())
				throw std::runtime_error("Material name is empty");

			builder.addSphere(name, position, radius, materialName, uv);

			//if (primitive.exists("rotateY")) {
			//	double angle = 0.0;
			//	primitive.lookupValue("rotateY", angle);
			//	//builder.rotateY(angle);
			//}
			//if (primitive.exists("translate")) {
			//	point3 translation = { 0.0, 0.0, 0.0 };
			//	translation = this->getPoint3d(primitive["translate"]);
			//	//builder.translate(translation);
			//}
		}
	}

	if (setting.exists("planes"))
	{
		for (int i = 0; i < setting["planes"].getLength(); i++)
		{
			const libconfig::Setting& primitive = setting["planes"][i];
			string name = "";
			point3 point1{};
			point3 point2{};
			std::string materialName = "";
			uvmapping uv = { 1, 1, 0, 0 };

			if (primitive.exists("name"))
				primitive.lookupValue("name", name);
			if (primitive.exists("point1"))
				point1 = this->getPoint(primitive["point1"]);
			if (primitive.exists("point2"))
				point2 = this->getPoint(primitive["point2"]);
			if (primitive.exists("material"))
				primitive.lookupValue("material", materialName);
			if (primitive.exists("uvmapping"))
				uv = this->getUVmapping(primitive["uvmapping"]);

			if (materialName.empty())
				throw std::runtime_error("Material name is empty");

			builder.addPlane(name, point1, point2, materialName, uv);

			//if (primitive.exists("rotateY")) {
			//	double angle = 0.0;
			//	primitive.lookupValue("rotateY", angle);
			//	//builder.rotateY(angle);
			//}
			//if (primitive.exists("translate")) {
			//	point3 translation = { 0.0, 0.0, 0.0 };
			//	translation = this->getPoint(primitive["translate"]);
			//	//builder.translate(translation);
			//}
		}
	}

	if (setting.exists("boxes"))
	{
		for (int i = 0; i < setting["boxes"].getLength(); i++)
		{
			const libconfig::Setting& primitive = setting["boxes"][i];
			string name = "";
			point3 position{};
			point3 size{};
			std::string materialName = "";
			uvmapping uv = {1, 1, 0, 0};

			if (primitive.exists("name"))
				primitive.lookupValue("name", name);
			if (primitive.exists("position"))
				position = this->getPoint(primitive["position"]);
			if (primitive.exists("size"))
				size = this->getPoint(primitive["size"]);
			if (primitive.exists("material"))
				primitive.lookupValue("material", materialName);
			if (primitive.exists("uvmapping"))
				uv = this->getUVmapping(primitive["uvmapping"]);
			
			if (materialName.empty())
				throw std::runtime_error("Material name is empty");

			builder.addBox(name, position, size, materialName, uv);

			if (primitive.exists("transform"))
			{
				rt::transform transform = this->getTransform(primitive["transform"]);

				if (transform.hasTranslate())
					builder.translate(transform.getTranslate());
				if (transform.hasRotate())
					builder.rotate(transform.getRotate());
				if (transform.hasScale())
					builder.scale(transform.getScale());

			}

			//if (primitive.exists("rotateY")) {
			//	double angle = 0.0;
			//	primitive.lookupValue("rotateY", angle);
			//	//builder.rotateY(angle);
			//}
			//if (primitive.exists("translate")) {
			//	point3 translation = { 0.0, 0.0, 0.0 };
			//	translation = this->getPoint3d(primitive["translate"]);
			//	//builder.translate(translation);
			//}
		}
	}

	if (setting.exists("cones"))
	{
		for (int i = 0; i < setting["cones"].getLength(); i++)
		{
			const libconfig::Setting& primitive = setting["cones"][i];
			string name = "";
			point3 position{};
			double radius = 0.0;
			double height = 0.0;
			std::string materialName = "";
			uvmapping uv = { 1, 1, 0, 0 };

			if (primitive.exists("name"))
				primitive.lookupValue("name", name);
			if (primitive.exists("position"))
				position = this->getPoint(primitive["position"]);
			if (primitive.exists("radius"))
				primitive.lookupValue("radius", radius);
			if (primitive.exists("height"))
				primitive.lookupValue("height", height);
			if (primitive.exists("material"))
				primitive.lookupValue("material", materialName);
			if (primitive.exists("uvmapping"))
				uv = this->getUVmapping(primitive["uvmapping"]);

			if (materialName.empty())
				throw std::runtime_error("Material name is empty");

			builder.addCone(name, position, radius, height, materialName, uv);

			//if (primitive.exists("rotateY")) {
			//	double angle = 0.0;
			//	primitive.lookupValue("rotateY", angle);
			//	//builder.rotateY(angle);
			//}
			//if (primitive.exists("translate")) {
			//	point3 translation = { 0.0, 0.0, 0.0 };
			//	translation = this->getPoint(primitive["translate"]);
			//	//builder.translate(translation);
			//}
		}
	}

	if (setting.exists("cylinders"))
	{
		for (int i = 0; i < setting["cylinders"].getLength(); i++)
		{
			const libconfig::Setting& primitive = setting["cylinders"][i];
			string name = "";
			point3 position{};
			double radius = 0.0;
			double height = 0.0;
			std::string materialName = "";
			uvmapping uv = { 1, 1, 0, 0 };

			if (primitive.exists("name"))
				primitive.lookupValue("name", name);
			if (primitive.exists("position"))
				position = this->getPoint(primitive["position"]);
			if (primitive.exists("radius"))
				primitive.lookupValue("radius", radius);
			if (primitive.exists("height"))
				primitive.lookupValue("height", height);
			if (primitive.exists("material"))
				primitive.lookupValue("material", materialName);
			if (primitive.exists("uvmapping"))
				uv = this->getUVmapping(primitive["uvmapping"]);

			if (materialName.empty())
				throw std::runtime_error("Material name is empty");

			builder.addCylinder(name, position, radius, height, materialName, uv);

			//if (primitive.exists("rotateY")) {
			//	double angle = 0.0;
			//	primitive.lookupValue("rotateY", angle);
			//	//builder.rotateY(angle);
			//}
			//if (primitive.exists("translate")) {
			//	point3 translation = { 0.0, 0.0, 0.0 };
			//	translation = this->getPoint(primitive["translate"]);
			//	//builder.translate(translation);
			//}
		}

		if (setting.exists("disks"))
		{
			for (int i = 0; i < setting["disks"].getLength(); i++)
			{
				const libconfig::Setting& primitive = setting["disks"][i];
				string name = "";
				point3 position{};
				double radius = 0.0;
				double height = 0.0;
				std::string materialName = "";
				uvmapping uv = { 1, 1, 0, 0 };

				if (primitive.exists("name"))
					primitive.lookupValue("name", name);
				if (primitive.exists("position"))
					position = this->getPoint(primitive["position"]);
				if (primitive.exists("radius"))
					primitive.lookupValue("radius", radius);
				if (primitive.exists("height"))
					primitive.lookupValue("height", height);
				if (primitive.exists("material"))
					primitive.lookupValue("material", materialName);
				if (primitive.exists("uvmapping"))
					uv = this->getUVmapping(primitive["uvmapping"]);

				if (materialName.empty())
					throw std::runtime_error("Material name is empty");

				builder.addDisk(name, position, radius, height, materialName, uv);

				//if (primitive.exists("rotateY")) {
				//	double angle = 0.0;
				//	primitive.lookupValue("rotateY", angle);
				//	//builder.rotateY(angle);
				//}
				//if (primitive.exists("translate")) {
				//	point3 translation = { 0.0, 0.0, 0.0 };
				//	translation = this->getPoint(primitive["translate"]);
				//	//builder.translate(translation);
				//}
			}
		}

		if (setting.exists("toruses"))
		{
			for (int i = 0; i < setting["toruses"].getLength(); i++)
			{
				const libconfig::Setting& primitive = setting["toruses"][i];
				string name = "";
				point3 position{};
				double major_radius = 0.0;
				double minor_radius = 0.0;
				std::string materialName = "";
				uvmapping uv = { 1, 1, 0, 0 };

				if (primitive.exists("name"))
					primitive.lookupValue("name", name);
				if (primitive.exists("position"))
					position = this->getPoint(primitive["position"]);
				if (primitive.exists("major_radius"))
					primitive.lookupValue("major_radius", major_radius);
				if (primitive.exists("minor_radius"))
					primitive.lookupValue("minor_radius", minor_radius);
				if (primitive.exists("material"))
					primitive.lookupValue("material", materialName);
				if (primitive.exists("uvmapping"))
					uv = this->getUVmapping(primitive["uvmapping"]);

				if (materialName.empty())
					throw std::runtime_error("Material name is empty");

				builder.addTorus(name, position, major_radius, minor_radius, materialName, uv);

				//if (primitive.exists("rotateY")) {
				//	double angle = 0.0;
				//	primitive.lookupValue("rotateY", angle);
				//	//builder.rotateY(angle);
				//}
				//if (primitive.exists("translate")) {
				//	point3 translation = { 0.0, 0.0, 0.0 };
				//	translation = this->getPoint(primitive["translate"]);
				//	//builder.translate(translation);
				//}
			}
		}
	}
}