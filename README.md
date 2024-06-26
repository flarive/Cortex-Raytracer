# Cortex-Raytracer

Cortex RT is a self made CPU based monte carlo path tracer created from scratch.

![Screenshot](images/cortex-raytracer-main.jpg)


# Primitives

Cortex supports an extended list of builtin uv mapped primitives mostly for testing purposes :
- Plane
- Cylinder
- Disk
- Cone
- Box
- Torus
- Sphere

![Screenshot of all builtin primitives](images/primitives.png)

[extended_primitives.scene](https://github.com/flarive/Cortex-Raytracer/blob/master/scenes/extended_primitives.scene)

# Meshes

Cortex RT supports loading Wavefront .obj files.

obj is a kind of old legacy plain text 3D file format but it is still widely used by 3D software (blender, 3ds max, maya...) as an export file format.
- .obj file contains vertex, faces, normals, uv mapping and smmothing groups definition
- .mtl (Material template library) additional file contains materials definition (color, textures, ambient, diffuse, specular, bump...)

If you want to knowm more about obj and mtl files : https://en.wikipedia.org/wiki/Wavefront_.obj_file



teapot.obj solid color | teapot.obj textured | 3ds max obj exporter
--- | --- | ---
![](images/teapot_obj_solid_color.png) | ![](images/teapot_obj_texture.png) | ![Screenshot of 3ds max OBJ file exporter](images/3dsmax_obj_exporter.png)




# Scenes

Cortex RT comes with a convenient .scene configuration file system to describe the scene to be rendered (primitives, meshes, camera, lights, materials, textures, rendering quality...)

```
# teapot_mesh.scene

image:
{
    width = 512;
    height = 388;
    maxDepth = 100;
    samplesPerPixel = 100;
    background = 
    {
        color = { r = 0.0; g = 0.0; b = 0.0; };
    };
};

# Configuration of the camera
camera:
{
    aspectRatio = "16:9";
    lookFrom = { x = 0.0; y = 4.0; z = 9.0; };
    lookAt = { x = 0.0; y = 0.0; z = 0.0; };
    upAxis = { x = 0.0; y = 1.0; z = 0.0; };
    fov = 18.0;
};

# Lights in the scene
lights:
{
    sphereLights: (
        {
            name = "MySphereLight";
            position = { x = 0.0; y = 8.0; z = 5.0; };
            radius = 3.0;
            intensity = 3.0;
            color = { r = 1.0; g = 1.0; b = 1.0; };
            invisible = true;
        }
    )
}

textures:
{
    solidColor: (
        {
            name = "ground_texture";
            color = { r = 0.7; g = 0.8; b = 1.0; };
        },
    ),
    image: (
        {
            name = "teapot_diffuse_texture";
            filepath = "../../data/models/rusted_metal1.jpg";
        }
    )
}

materials:
{
    lambertian: (
        {
            name = "ground_material";
            texture = "ground_texture";
        }
    ),
    phong: (
        {
            name = "teapot_material";
            diffuseTexture = "teapot_diffuse_texture";
            ambientColor = { r = 0.0; g = 0.0; b = 0.0; };
            shininess = 0.9;
        }
    )
}

# Meshes in the scene
meshes:
{
    obj: (
        {
            name = "MyTeapotModel"
            filepath = "../../data/models/teapot.obj";
            position = { x = 0.0; y = 0.0; z = 0.0; };
            material = "teapot_material"
            use_mtl = false;
            use_smoothing = true;
            transform =
            {
                translate = { x = 0.0; y = 0.0; z = 0.0; };
                rotate = { x = 0.0; y = 0.0; z = 0.0; };
                scale = { x = 0.02; y = 0.02; z = 0.02; };
            };
        }
    );
};

# Primitives in the scene
primitives:
{
    # List of boxes
    boxes: (
        {
            name = "MyGround";
            position = { x = 0.0; y = -1.0; z = 0.0; };
            size = { x = 40.0; y = 0.5; z = 40.0; };
            material = "ground_material";
            uvmapping = { scale_u = 1.0; scale_v = 1.0; offset_u = 0.0; offset_v = 0.0; repeat_u = 2.0; repeat_v = 2.0; };
        }
    );
};
```

# Materials

![Screenshot of all suported materials](images/all_materials.jpg)

Cortex RT mostly support following materials :
- Phong
- Oren Nayar
- Anisotropic (for wood, slate, ice, velvet...)
- Isotropic (for glass, crystals with cubic symmetry, diamonds, plastic, metal...)
- Dielectric (for water, glass, diamond...)
- Lambertian (simple diffuse material without any specular relection)
- Metal (for metals, mirrors...)
- Emissive (WIP)

crate.obj (diffuse + specular + bump textures) | cushion.obj (diffuse + specular + normal textures)
--- | ---
![Screenshot of phong crate model with diffuse + specular + bump textures](images/crate_diffuse_specular_bump.png) | ![Screenshot of phong cushion model with diffuse + specular + normal textures](images/cushion_normal.png)
![Screenshot of crate textures](images/crate_textures.png) | ![Screenshot of cushion textures](images/cushion_textures.png)

The phong reflection model is a full featured material that combines ambient + diffuse + specular.
https://en.wikipedia.org/wiki/Phong_reflection_model

On top of that, Cortex RT phong material also support a lot of possible textures (solid color, gradient, image, bump, normal, displacement, alpha...)

```
# Sample materials section in scene file
materials:
{
    lambertian: (
        {
            name = "ground_material";
            texture = "ground_texture";
        }
    ),
    phong: (
        {
            name = "crate_material";
            diffuseTexture = "crate_diffuse_texture";
            specularTexture = "crate_specular_texture";
            #bumpTexture = "crate_bump_texture";
            normalTexture = "crate_normal_texture";
            ambientColor = { r = 0.0; g = 0.0; b = 0.0; };
            shininess = 0.0;
        }
    )
}
```
[all_materials_spheres.scene](https://github.com/flarive/Cortex-Raytracer/blob/master/scenes/all_materials_spheres.scene)

# Textures

![Screenshot of all suported materials](images/all_textures.jpg)

Cortex RT supports quite a lot of textures you can combine in a material :

- Solid color texture
- Gradient color texture
- Image texture
- Bump texture
- Normal texture
- Displacement texture (WIP)
- Alpha texture (transparency texture)
- Perlin texture (procedural noise)
- Checker (procedural checkboard pattern texture)
- Emissive texture (WIP)

```
# Sample textures section in scene file
textures:
{
    image: (
        {
            name = "ground_texture";
            filepath = "../../data/textures/scifi_floor2.png";
        },
        {
            name = "crate_diffuse_texture";
            filepath = "../../data/models/crate_diffuse.jpg";
        },
        {
            name = "crate_specular_texture";
            filepath = "../../data/models/crate_specular.jpg";
        },
        {
            name = "crate_bump_texture";
            filepath = "../../data/models/crate_bump.jpg";
        }
    ),
    normal: (
        {
            name = "crate_normal_texture";
            filepath = "../../data/models/crate_normal.jpg";
            strength = 0.005;
        }
    )
}
```

[all_textures_spheres.scene](https://github.com/flarive/Cortex-Raytracer/blob/master/scenes/all_textures_spheres.scene)

Bump texture | Normal texture
--- | ---
![Screenshot of bump texture](images/bump_demo.png) | ![Screenshot of normal texture](images/normal_demo.png)




# Camera

Cortex RT provides 2 kinds of camera : perspective (default) and orthographic (no perception of depth)

Perspective camera | Orthographic camera
--- | ---
![](images/camera_perspective.png) | ![](images/camera_orthographic.png)

```
# Configuration of a perspective camera
camera:
{
    aspectRatio = "16:9";
    lookFrom = { x = 0.0; y = 2.0; z = 9.0; };
    lookAt = { x = 0.0; y = 0.6; z = 0.0; };
    upAxis = { x = 0.0; y = 1.0; z = 0.0; };
    fov = 18.0; // Field Of View (zoom level in degrees)
};
```

```
# Configuration of an orthographic camera
camera:
{
    aspectRatio = "16:9";
    lookFrom = { x = 0.0; y = 2.0; z = 9.0; };
    lookAt = { x = 0.0; y = 0.6; z = 0.0; };
    upAxis = { x = 0.0; y = 1.0; z = 0.0; };
    orthographic = true;
    orthoHeight = 2.0; // Orthographic viewport height (height of the viewport in world space units)
};
```



# Background

# Image settings

# Lights

Cortex RT supports 3 different kinds of lights :

- Omni light

Also known as a point light, an omni light emits light equally in all directions from a single point in space, similar to a light bulb.

- Directional light

A directional light simulates light coming from a specific direction, as if it were emitted from an infinitely large source, such as the sun

- Spot light

A spot light is a more focused light source that emits light in a specific direction within a cone


Lights are produced by hidden objects (quad or sphere) with an emissive material.

Such objects can be displayed for debug lightning purpose.

Omni light | Omni light debug (emissive sphere)
--- | ---
![](images/omni_light.png) | ![](images/omni_light_debug.png)

Directional light | Directional light debug (emissive quad)
--- | ---
![](images/directional_light.png) | ![](images/directional_light_debug.png)

Spot light | Spot light debug (emissive sphere)
--- | ---
![](images/spot_light.png) | ![](images/spot_light_debug.png)

[all_lights_types.scene](https://github.com/flarive/Cortex-Raytracer/blob/master/scenes/all_lights_types.scene)

# Transforms

# Groups

# Rendering parameters

# Anti aliasing

# Performances and optimizations

# Command line exe

# Graphical User Interface

# Dependencies

# Thanks to

# What's next ?



