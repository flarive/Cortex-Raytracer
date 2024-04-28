#include "Configuration.h"

#include "../textures/image_texture.h"
#include "../textures/solid_color_texture.h"

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
				color = this->getRGB(texture["color"]);

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
	//if (setting.exists("directionalLight"))
	//{
	//	if (setting["directionalLight"].exists("textured"))
	//	{
	//		for (int i = 0; i < setting["directionalLight"]["textured"].getLength(); i++)
	//		{
	//			const libconfig::Setting& material = setting["directionalLight"]["textured"][i];
	//			std::string name = "";
	//			std::string textureName = "";

	//			if (material.exists("name"))
	//				material.lookupValue("name", name);
	//			if (material.exists("texture"))
	//				material.lookupValue("texture", textureName);

	//			if (name.empty())
	//				throw std::runtime_error("Material name is empty");
	//			if (textureName.empty())
	//				throw std::runtime_error("Texture name is empty");

	//			//builder.addDirectionalLightMaterial(name, textureName);
	//		}
	//	}
	//	if (setting["directionalLight"].exists("colored"))
	//	{
	//		for (int i = 0; i < setting["directionalLight"]["colored"].getLength();	i++)
	//		{
	//			const libconfig::Setting& material = setting["directionalLight"]["colored"][i];
	//			std::string name = "";
	//			color color = { 0.0, 0.0, 0.0 };

	//			if (material.exists("name"))
	//				material.lookupValue("name", name);
	//			if (material.exists("color"))
	//				color = this->getRGB(material["color"]);

	//			if (name.empty())
	//				throw std::runtime_error("Material name is empty");

	//			//builder.addDirectionalLightMaterial(name, color);
	//		}
	//	}
	//}

	if (setting.exists("diffuse"))
	{
		for (int i = 0; i < setting["diffuse"].getLength(); i++)
		{
			const libconfig::Setting& material = setting["diffuse"][i];
			std::string name{};
			color rgb{};
			std::string textureName{};

			if (material.exists("name"))
				material.lookupValue("name", name);
			if (material.exists("color"))
				rgb = this->getRGB(material["color"]);
			if (material.exists("texture"))
				material.lookupValue("texture", textureName);

			if (name.empty())
				throw std::runtime_error("Material name is empty");

			if (!textureName.empty())
				builder.addSolidMaterial(name, textureName);
			else
				builder.addSolidMaterial(name, rgb);
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

			//builder.addGlassMaterial(name, refraction);
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
				color = this->getRGB(material["color"]);
			if (material.exists("fuzziness"))
				material.lookupValue("fuzziness", fuzziness);
			if (name.empty())
				throw std::runtime_error("Material name is empty");

			//builder.addMetalMaterial(name, color, fuzziness);
		}
	}
	if (setting.exists("solid"))
	{
		if (setting["solid"].exists("textured"))
		{
			for (int i = 0; i < setting["solid"]["textured"].getLength(); i++)
			{
				const libconfig::Setting& material = setting["solid"]["textured"][i];
				std::string name = "";
				std::string textureName = "";

				if (material.exists("name"))
					material.lookupValue("name", name);
				if (material.exists("texture"))
					material.lookupValue("texture", textureName);

				if (name.empty())
					throw std::runtime_error("Material name is empty");
				if (textureName.empty())
					throw std::runtime_error("Texture name is empty");

				//builder.addSolidMaterial(name, textureName);
			}
		}
		if (setting["solid"].exists("colored"))
		{
			for (int i = 0; i < setting["solid"]["colored"].getLength(); i++)
			{
				const libconfig::Setting& material = setting["solid"]["colored"][i];
				std::string name = "";
				color color = { 0.0, 0.0, 0.0 };

				if (material.exists("name"))
					material.lookupValue("name", name);
				if (material.exists("color"))
					color = this->getRGB(material["color"]);

				if (name.empty())
					throw std::runtime_error("Material name is empty");

				//builder.addSolidMaterial(name, color);
			}
		}
	}
}

point3 Configuration::getPoint3d(const libconfig::Setting& setting)
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
		builder.imageBackgroundColor(getRGB(setting["backgroundColor"]));
}

color Configuration::getRGB(const libconfig::Setting& setting)
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
		builder.cameraLookFrom(this->getPoint3d(setting["lookFrom"]));
	}
	if (setting.exists("lookAt")) {
		builder.cameraLookAt(this->getPoint3d(setting["lookAt"]));
	}
	if (setting.exists("upAxis")) {
		builder.cameraUpAxis(this->getPoint3d(setting["upAxis"]));
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
			point3 position = { 0.0, 0.0, 0.0 };
			double radius = 0.0;
			std::string materialName = "";

			if (primitive.exists("name"))
				primitive.lookupValue("name", name);
			if (primitive.exists("position"))
				position = this->getPoint3d(primitive["position"]);
			if (primitive.exists("radius"))
				primitive.lookupValue("radius", radius);
			if (primitive.exists("material"))
				primitive.lookupValue("material", materialName);

			if (materialName.empty())
				throw std::runtime_error("Material name is empty");

			builder.addSphere(name, position, radius, materialName);

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
			point3 point1 = { 0.0, 0.0, 0.0 };
			point3 point2 = { 0.0, 0.0, 0.0 };
			std::string materialName = "";

			if (primitive.exists("point1"))
				point1 = this->getPoint3d(primitive["point1"]);
			if (primitive.exists("point2"))
				point2 = this->getPoint3d(primitive["point2"]);
			if (primitive.exists("material"))
				primitive.lookupValue("material", materialName);

			if (materialName.empty())
				throw std::runtime_error("Material name is empty");
			//builder.addPlane(point1, point2, materialName);

			if (primitive.exists("rotateY")) {
				double angle = 0.0;
				primitive.lookupValue("rotateY", angle);
				//builder.rotateY(angle);
			}
			if (primitive.exists("translate")) {
				point3 translation = { 0.0, 0.0, 0.0 };
				translation = this->getPoint3d(primitive["translate"]);
				//builder.translate(translation);
			}
		}
	}

	if (setting.exists("boxes"))
	{
		for (int i = 0; i < setting["boxes"].getLength(); i++)
		{
			const libconfig::Setting& primitive = setting["boxes"][i];
			string name = "";
			point3 position = { 0.0, 0.0, 0.0 };
			point3 size = { 0.0, 0.0, 0.0 };
			std::string materialName = "";

			if (primitive.exists("name"))
				primitive.lookupValue("name", name);
			if (primitive.exists("position"))
				position = this->getPoint3d(primitive["position"]);
			if (primitive.exists("size"))
				size = this->getPoint3d(primitive["size"]);
			if (primitive.exists("material"))
				primitive.lookupValue("material", materialName);

			if (materialName.empty())
				throw std::runtime_error("Material name is empty");

			builder.addBox(name, position, size, materialName);

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
			point3 position = { 0.0, 0.0, 0.0 };
			double radius = 0.0;
			double height = 0.0;
			std::string materialName = "";

			if (primitive.exists("position"))
				position = this->getPoint3d(primitive["position"]);
			if (primitive.exists("radius"))
				primitive.lookupValue("radius", radius);
			if (primitive.exists("height"))
				primitive.lookupValue("height", height);
			if (primitive.exists("material"))
				primitive.lookupValue("material", materialName);

			if (materialName.empty())
				throw std::runtime_error("Material name is empty");

			//builder.addCone(position, radius, height, materialName);

			if (primitive.exists("rotateY")) {
				double angle = 0.0;
				primitive.lookupValue("rotateY", angle);
				//builder.rotateY(angle);
			}
			if (primitive.exists("translate")) {
				point3 translation = { 0.0, 0.0, 0.0 };
				translation = this->getPoint3d(primitive["translate"]);
				//builder.translate(translation);
			}
		}
	}

	if (setting.exists("cylinders"))
	{
		for (int i = 0; i < setting["cylinders"].getLength(); i++)
		{
			const libconfig::Setting& primitive = setting["cylinders"][i];
			point3 position = { 0.0, 0.0, 0.0 };
			double radius = 0.0;
			double height = 0.0;
			std::string materialName = "";

			if (primitive.exists("position"))
				position = this->getPoint3d(primitive["position"]);
			if (primitive.exists("radius"))
				primitive.lookupValue("radius", radius);
			if (primitive.exists("height"))
				primitive.lookupValue("height", height);
			if (primitive.exists("material"))
				primitive.lookupValue("material", materialName);

			if (materialName.empty())
				throw std::runtime_error("Material name is empty");

			//builder.addCylinder(position, radius, height, materialName);

			if (primitive.exists("rotateY")) {
				double angle = 0.0;
				primitive.lookupValue("rotateY", angle);
				//builder.rotateY(angle);
			}
			if (primitive.exists("translate")) {
				point3 translation = { 0.0, 0.0, 0.0 };
				translation = this->getPoint3d(primitive["translate"]);
				//builder.translate(translation);
			}
		}
	}
}