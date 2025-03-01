#pragma once

#include "../primitives/hittable.h"
#include "../primitives/hittable_list.h"

#include <memory>
#include <vector>

class camera;

class scene
{
public:
	scene();
	//scene(std::shared_ptr<camera> _camera);
	void set(const hittable_list& _objects);
	void add(std::shared_ptr<hittable> _object);
	void set_camera(std::shared_ptr<camera> _camera);

	const hittable_list& get_world();
	const hittable_list& get_emissive_objects();
	std::shared_ptr<camera> get_camera();

	//const std::vector<SpotLight> get_lights();

	void extract_emissive_objects();
	void build_optimized_world(randomizer& rnd);



    typedef struct {
        color rgb;
        std::string filepath;
        bool is_skybox;
    } imageBackgroundConfig;

    typedef struct {
        int height;
        int width;
        int depth;
        int spp;
        imageBackgroundConfig background;
        std::string outputFilePath;
    } imageConfig;

    typedef struct {
        double aspectRatio;
        double openingTime; // ???? used ?
        point3 lookFrom;
        point3 lookAt;
        point3 upAxis;
        double aperture; // for depth of field (defocus_angle)
        double focusDist; // for depth of field
        bool isOrthographic;
        double verticalFov; // for perspective cams
        double orthoHeight; // for orthographic cams
    } cameraConfig;


private:
	hittable_list m_world;
	std::shared_ptr<camera> m_camera;
	hittable_list m_emissive_objects;
};