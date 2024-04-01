#include "Renderer.h"

#include <omp.h>

#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>


Renderer::Renderer(std::unique_ptr<scene> scene, Random::SeedType randomSeed) :
	m_samplesPerPixels(100),
	m_maxDepth(100),
	m_minT(0.0001),
	m_randomSeed(randomSeed),
	m_scene(std::move(scene))
{
	sqrt_spp = static_cast<int>(sqrt(m_samplesPerPixels));
	recip_sqrt_spp = 1.0 / sqrt_spp;
}

int Renderer::samplesPerPixels() const
{
	return m_samplesPerPixels;
}

int Renderer::maxDepth() const
{
	return m_maxDepth;
}

void Renderer::setSamplesPerPixels(int samplesPerPixels)
{
	m_samplesPerPixels = samplesPerPixels;

	sqrt_spp = static_cast<int>(sqrt(m_samplesPerPixels));
	recip_sqrt_spp = 1.0 / sqrt_spp;
}

void Renderer::setMaxDepth(int maxDepth)
{
	m_maxDepth = maxDepth;
}

FloatImage Renderer::render(int width, int height)
{
	auto floatImage = renderWithoutGammaCorrection(width, height);

	// Multi-sampling and Gamma correction, with gamma=2.0
	floatImage /= static_cast<double>(m_samplesPerPixels);
	floatImage.applyGammaCorrection();

	return floatImage;
}

FloatImage Renderer::renderWithoutGammaCorrection(int width, int height)
{
	const auto nbThreads = omp_get_max_threads();

	// Random number generators per thread, for reproducibility
	auto randomGenerators = Random::createMultiple(nbThreads, m_randomSeed);

	// 2D array of colors per thread
	std::vector<FloatImage> floatImages(nbThreads, FloatImage(width, height));

	// The progress is the number of horizontal scan lines times the number of samples
	const long totalProgress = m_samplesPerPixels * height;
	long progress = 0;


	#pragma omp parallel for
	for (int j = 0; j < height; j++)
	{
		const auto currentThread = omp_get_thread_num();

		std::cout << "row " << j << std::endl;
			
		for (int i = 0; i < width; i++)
		{
			auto& randomGenerator = randomGenerators[currentThread];

			// Compute normalized coordinates (randomized), y is reversed in the image
			//const auto x = (static_cast<double>(i) + randomGenerator.randomNumber()) / (width - 1);
			//const auto y = (static_cast<double>(height - 1 - j) + randomGenerator.randomNumber()) / (height - 1);

			// each pixel is black at the beginning
			color pixel_color(0, 0, 0);
				

			for (int s_j = 0; s_j < sqrt_spp; ++s_j)
			{
				for (int s_i = 0; s_i < sqrt_spp; ++s_i)
				{
					// Generate a ray from the camera
					if (i == 81 && j == 67)
					{
						int ss = 0;
					}
						
					const auto ray = m_scene->get_camera()->get_ray(i, j, s_i, s_j);

					// Add colors for each sample per pixel
					pixel_color += ray_color(ray, m_maxDepth, randomGenerator);

					//std::cout << "color " << pixel_color << std::endl;
					
				}
			}

			floatImages[currentThread].at(j, i) = vector3(pixel_color.r(), pixel_color.g(), pixel_color.b()); // dirty
		}

		// Update progress after each full line
		#pragma omp atomic
		progress++;

		// The main thread is responsible for displaying the progress
		if (currentThread == 0)
		{
			// TODO: display the progress each second, and the ETA
			// Use string stream to prevent some of the concurrency issues when displaying
			std::stringstream progressStream;
			//progressStream << "Progress: " << static_cast<double>(progress) / static_cast<double>(totalProgress) << std::endl;
			std::cout << progressStream.str();
		}
	}
	

	// Aggregate the images from each CPU core
	return FloatImage::aggregateImages(floatImages);
}
//
//vector3 Renderer::computeRayColor(const ray& ray, int depth, Random& randomGenerator) const
//{	
//	vector3 color(0.0, 0.0, 0.0);
//
//	// Ray bounce limit is not exceeded yet
//	if (depth > 0)
//	{
//		// Compute intersection with meshes
//		hit_record hit;
//		if (rayMeshesIntersection(m_scene->meshes(), ray, m_minT, hit))
//		{
//			Vec3 attenuation;
//			Ray scatteredRay;
//			
//			if (hit.material->scatter(hit, randomGenerator, attenuation, scatteredRay))
//			{
//				return attenuation * computeRayColor(scatteredRay, depth - 1, randomGenerator);
//			}
//		}
//		else
//		{
//			// No intersection, display the background color
//			Vec3 unitDirection = glm::normalize(ray.direction());
//			const auto t = 0.5 * (unitDirection.y + 1.0);
//			return (1.0 - t) * Vec3(1.0, 1.0, 1.0) + t * Vec3(0.5, 0.7, 1.0);
//		}
//	}
//
//	return color;
//}

/// <summary>
/// Fire a given ray and get the hit record (recursive)
/// </summary>
/// <param name="r"></param>
/// <param name="world"></param>
/// <returns></returns>
color Renderer::ray_color(const ray& r, int depth, Random& randomGenerator)
{
	hit_record rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
	{
		// return black color
		return background;
	}


	// If the ray hits nothing, return the background color.
	// 0.001 is to fix shadow acne interval
	if (!m_scene->get_world().hit(r, interval(SHADOW_ACNE_FIX, infinity), rec, depth))
	{
		return background;
	}



	// ray hit a world object
	scatter_record srec;
	color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.hit_point);


	// hack for invisible primitives (such as lights)
	if (color_from_emission.a() == 0)
	{
		m_scene->get_world().hit(r, interval(rec.t + 0.001, infinity), rec, depth);
	}

	if (!rec.mat->scatter(r, m_scene->get_lights(), rec, srec))
	{
		return color_from_emission;
	}

	if (m_scene->get_lights().objects.size() == 0)
	{
		// no lights
		// no importance sampling
		return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, randomGenerator);
	}

	// no importance sampling
	if (srec.skip_pdf)
	{
		return srec.attenuation * ray_color(srec.skip_pdf_ray, depth - 1, randomGenerator);
	}

	std::shared_ptr< hittable_pdf> light_ptr = std::make_shared<hittable_pdf>(m_scene->get_lights(), rec.hit_point);
	mixture_pdf p(light_ptr, srec.pdf_ptr);

	ray scattered = ray(rec.hit_point, p.generate(), r.time());
	auto pdf_val = p.value(scattered.direction());

	double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);

	color sample_color = ray_color(scattered, depth - 1, randomGenerator);
	color color_from_scatter = (srec.attenuation * scattering_pdf * sample_color) / pdf_val;

	return color_from_emission + color_from_scatter;
}


bool Renderer::saveAsPPM(const FloatImage& image, const std::string& filename)
{
	std::ofstream file(filename);

	if (!file.is_open())
	{
		return false;
	}

	file << "P3\n" << image.width() << ' ' << image.height() << "\n255\n";

	for (int i = 0; i < image.height(); i++)
	{
		for (int j = 0; j < image.width(); j++)
		{
			// Conversion to RGB in [0; 255]
			const auto red = static_cast<int>(255.0 * std::clamp(image.at(i, j).x, 0.0, 1.0));
			const auto green = static_cast<int>(255.0 * std::clamp(image.at(i, j).y, 0.0, 1.0));
			const auto blue = static_cast<int>(255.0 * std::clamp(image.at(i, j).z, 0.0, 1.0));

			file << red << ' ' << green << ' ' << blue << '\n';
		}
	}

	file.close();

	return false;
}
