#pragma once

#include <array>
#include <filesystem>
#include <string>
#include <vector>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "obj/tinyobjloader.hpp"

#include "../misc/ray.h"
#include "hittable.h"
#include "../utilities/math_utils.h"
#include "../utilities/types.h"

struct mesh_vertex
{
	vector3 position;
	vector3 normal;

	mesh_vertex(const vector3& position, const vector3& normal) :
		position(position),
		normal(normal)
	{
		
	}
};

struct mesh_face
{
	std::array<int, 3> vertices;
	std::array<int, 3> normals;

	mesh_face(const std::array<int, 3>& vertices, const std::array<int, 3>& normals) :
		vertices(vertices),
		normals(normals)
	{

	}
};

class mesh : public hittable
{
public:
	mesh() :
		m_boundingBox({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }),
		m_material(nullptr)
	{
	}

	explicit mesh(std::vector<mesh_vertex> vertices, std::vector<mesh_face> faces) :
		m_vertices(std::move(vertices)),
		m_faces(std::move(faces)),
		m_boundingBox({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }),
		m_material(nullptr)
	{
		updateBoundingBox();
	}

	

	void setVertices(std::vector<mesh_vertex> vertices)
	{
		m_vertices = std::move(vertices);

		updateBoundingBox();
	}

	void setFaces(std::vector<mesh_face> faces)
	{
		m_faces = std::move(faces);
	}

	void setMaterial(std::shared_ptr<material> mat)
	{
		m_material = std::move(mat);
	}

	/// <summary>
	/// Apply a glm transformation to the primitive
	/// Such as translate, rotate, scale...
	/// </summary>
	/// <param name="transformation"></param>
	void applyTransformation(const matrix4& transformation)
	{
		const auto normalTransformation = glm::inverseTranspose(transformation);

		// Transform all vertices and normals
		for (unsigned int i = 0; i < m_vertices.size(); i++)
		{
			m_vertices[i].position = mapPoint(transformation, m_vertices[i].position);
			m_vertices[i].normal = mapVector(normalTransformation, m_vertices[i].normal);
		}

		updateBoundingBox();
	}

	int nbVertices() const
	{
		return static_cast<int>(m_vertices.size());
	}

	int nbFaces() const
	{
		return static_cast<int>(m_faces.size());
	}

	const std::vector<mesh_vertex>& vertices() const
	{
		return m_vertices;
	}


	const std::vector<mesh_face>& faces() const
	{
		return m_faces;
	}

	const std::shared_ptr<material>& materials() const
	{
		return m_material;
	}

	aabb bounding_box() const override
	{
		return m_boundingBox;
	}


	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		// Compute intersection with meshes
		return rayMeshIntersection(*this, r, ray_t.min, rec);
	}


	/**
	 * \brief Return the transformed vertex v (between 0 and 2) from a face
	 * \param face The index of the face of the mesh
	 * \param v The index of the vertex in the face
	 * \return The 3D coordinates of the vertex
	 */
	vector3 vertex(int face, int v) const
	{
		assert(face >= 0 && face < nbFaces());
		assert(v >= 0 && v < 3);

		const auto index = m_faces[face].vertices[v];

		assert(index >= 0 && index < nbVertices());

		return m_vertices[index].position;
	}

	/**
	 * \brief Return the transformed normal at a vertex v (between 0 and 2) from a face
	 * \param face The index of the face of the mesh
	 * \param v The index of the vertex in the face
	 * \return The 3D coordinates of the normal
	 */
	vector3 normal(int face, int v) const
	{
		assert(face >= 0 && face < nbFaces());
		assert(v >= 0 && v < 3);

		const auto index = m_faces[face].normals[v];

		if (index >= 0 && index < nbVertices())
		{
			// Normal is defined in the mesh
			return m_vertices[index].normal;
		}
		else
		{
			// We need to manually compute the normal
			const auto v0 = vertex(face, 0);
			const auto v1 = vertex(face, 1);
			const auto v2 = vertex(face, 2);

			return glm::normalize(glm::cross(v1 - v0, v2 - v0));
		}
	}

	/**
	 * \brief Return the transformed normal interpolated with barycentric coordinates from a face
	 * \param face The index of the face of the mesh
	 * \param u The first barycentric coordinate
	 * \param v The second barycentric coordinate
	 * \return The 3D coordinates of the normal
	 */
	vector3 normal(int face, double u, double v) const
	{
		const auto n0 = normal(face, 0);
		const auto n1 = normal(face, 1);
		const auto n2 = normal(face, 2);

		const vector3 normalVector = (1.0 - u - v) * n0 + u * n1 + v * n2;
		return glm::normalize(normalVector);
	}


	

private:
	/**
	 * \brief Update the internal AABB of the mesh.
	 *        Warning: run this when the mesh is updated.
	 */
	void updateBoundingBox() override
	{
		if (!m_vertices.empty())
		{
			vector3 a = m_vertices.front().position;
			vector3 b = m_vertices.front().position;

			for (const auto& v : m_vertices)
			{
				a = glm::min(a, v.position);
				b = glm::max(b, v.position);
			}

			m_boundingBox = aabb(a, b);
		}
	}

	static bool rayMeshIntersection(
		const mesh& mesh,
		const ray& ray,
		double minT,
		hit_record& hit)
	{
		// First intersect ray with AABB to quickly discard non-intersecting rays
		if (!rayAABBIntersection(mesh.bounding_box(), ray))
		{
			return false;
		}

		const auto& orig = ray.origin();
		const auto& dir = ray.direction();

		bool intersectionFound = false;

		//hit.ray = ray; // ??????????????????????????????????????????????????????????????????
		hit.t = std::numeric_limits<double>::max();

		// Iterate over all triangles in the mesh
		for (int f = 0; f < mesh.nbFaces(); f++)
		{
			const auto& v0 = mesh.vertex(f, 0);
			const auto& v1 = mesh.vertex(f, 1);
			const auto& v2 = mesh.vertex(f, 2);

			// Distance between origin and hit along the ray
			double t;
			// Output barycentric coordinates of the intersection point
			vector2 baryPosition;

			// Check if there is an intersection with this triangle
			if (glm::intersectRayTriangle(orig, dir, v0, v1, v2, baryPosition, t) && t >= minT && t < hit.t)
			{
				hit.t = t;
				hit.p = ray.at(hit.t);
				hit.normal = mesh.normal(f, baryPosition.x, baryPosition.y);
				hit.front_face = glm::dot(ray.direction(), hit.normal) < 0;
				hit.mat = mesh.materials();

				intersectionFound = true;
			}
		}

		return intersectionFound;
	}

	static bool rayMeshesIntersection(
		const std::vector<mesh>& meshes,
		const ray& ray,
		double minT,
		hit_record& hit)
	{
		bool intersectionFound = false;

		hit.t = std::numeric_limits<double>::max();

		// Iterate over all triangles in the mesh
		for (unsigned int m = 0; m < meshes.size(); m++)
		{
			hit_record meshHit;

			// Check if there is an intersection with this triangle
			if (rayMeshIntersection(meshes[m], ray, minT, meshHit) && meshHit.t < hit.t)
			{
				hit = meshHit;

				intersectionFound = true;
			}
		}

		return intersectionFound;
	}

	/**
	 * \brief Vertices and normals of the mesh
	 */
	std::vector<mesh_vertex> m_vertices;
	
	/**
	 * \brief Faces of the mesh
	 */
	std::vector<mesh_face> m_faces;

	/**
	 * \brief Bounding box of the mesh for ray intersection acceleration
	 */
	aabb m_boundingBox;
	
	/**
	 * \brief Material of the mesh
	 */
	std::shared_ptr<material> m_material;
};

int indexOfNumberLetter(const std::string& str, int offset)
{
	const auto length = static_cast<int>(str.length());

	for (int i = offset; i < length; ++i)
	{
		if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-' || str[i] == '.') return i;
	}

	return length;
}

int lastIndexOfNumberLetter(const std::string& str)
{
	const auto length = static_cast<int>(str.length());

	for (int i = length - 1; i >= 0; --i)
	{
		if ((str[i] >= '0' && str[i] <= '9') || str[i] == '-' || str[i] == '.') return i;
	}

	return length;
}

std::vector<std::string> split(const std::string& s, char delim)
{
	std::vector<std::string> elems;

	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	return elems;
}


bool loadMesh(const std::filesystem::path& filename, std::vector<mesh_vertex>& vertices, std::vector<mesh_face>& faces)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{
		std::cerr << "Mesh::load() - Could not open file " << filename << std::endl;
		return false;
	}

	// Store vertex and normal data while reading
	std::vector<vector3> raw_vertices;
	std::vector<vector3> raw_normals;
	std::vector<int> v_elements;
	std::vector<int> n_elements;

	std::string line;
	while (getline(file, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			// Read position data
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			auto values = split(line.substr(index1, index2 - index1 + 1), ' ');
			raw_vertices.emplace_back(stof(values[0]), stof(values[1]), stof(values[2]));
		}
		else if (line.substr(0, 3) == "vn ")
		{
			// Read normal data
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			auto values = split(line.substr(index1, index2 - index1 + 1), ' ');
			raw_normals.emplace_back(stof(values[0]), stof(values[1]), stof(values[2]));
		}
		else if (line.substr(0, 2) == "f ")
		{
			// Read face data
			int index1 = indexOfNumberLetter(line, 2);
			int index2 = lastIndexOfNumberLetter(line);
			auto values = split(line.substr(index1, index2 - index1 + 1), ' ');
			for (int i = 0; i < static_cast<int>(values.size()) - 2; i++)
			{
				// Split up vertex indices
				auto v1 = split(values[0], '/'); // Triangle fan for ngons
				auto v2 = split(values[i + 1], '/');
				auto v3 = split(values[i + 2], '/');

				// Store position indices
				v_elements.push_back(std::stoul(v1[0]) - 1);
				v_elements.push_back(std::stoul(v2[0]) - 1);
				v_elements.push_back(std::stoul(v3[0]) - 1);

				// Check for normals
				if (v1.size() >= 3 && v1[2].length() > 0)
				{
					n_elements.push_back(std::stoul(v1[2]) - 1);
					n_elements.push_back(std::stoul(v2[2]) - 1);
					n_elements.push_back(std::stoul(v3[2]) - 1);
				}
			}
		}
	}

	file.close();

	// Resize vertices and faces vectors
	vertices.clear();
	vertices.reserve(raw_vertices.size());

	for (unsigned int i = 0; i < std::max(raw_vertices.size(), raw_normals.size()); i++)
	{
		vector3 vertex;
		vector3 normal;

		if (i < raw_vertices.size())
		{
			vertex = raw_vertices[i];
		}

		if (i < raw_normals.size())
		{
			normal = raw_normals[i];
		}

		vertices.emplace_back(vertex, normal);
	}

	faces.clear();
	faces.reserve(std::max(v_elements.size(), n_elements.size()) / 3);

	for (unsigned int i = 0; i < std::max(v_elements.size(), n_elements.size()); i += 3)
	{
		std::array<int, 3> v = { {0, 0, 0} };
		std::array<int, 3> n = { {-1, -1, -1} };

		if (i + 2 < v_elements.size())
		{
			v[0] = v_elements[i];
			v[1] = v_elements[i + 1];
			v[2] = v_elements[i + 2];
		}

		if (i + 2 < n_elements.size())
		{
			n[0] = n_elements[i];
			n[1] = n_elements[i + 1];
			n[2] = n_elements[i + 2];
		}

		faces.emplace_back(v, n);
	}

	return true;
}

bool loadMesh(const std::filesystem::path& filename, mesh& mesh)
{
	std::vector<mesh_vertex> vertices;
	std::vector<mesh_face> faces;

	const auto success = loadMesh(filename, vertices, faces);

	mesh.setVertices(vertices);
	mesh.setFaces(faces);

	return success;
}


bool loadMeshTinyObj(const std::string& file_name, mesh& mesh)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_name.c_str());

	if (!warn.empty()) {
		std::cout << "[tinyobj warning] " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << "[tinyobj error] " << err << std::endl;
	}

	if (!ret)
	{
		exit(1);
	}

	std::vector<vector3> raw_vertices;
	std::vector<vector2> raw_uvs;
	std::vector<vector3> raw_normals;
	std::vector<int> indices;


	for (size_t s = 0; s < shapes.size(); s++)
	{
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			int fv = shapes[s].mesh.num_face_vertices[f];
			if (fv != 3)
			{
				std::cerr << "Only triangle faces allowed at this point" << std::endl;
				exit(1);
			}

			raw_vertices.resize(attrib.vertices.size());
			raw_normals.resize(attrib.vertices.size());
			raw_uvs.resize(attrib.vertices.size());

			for (int i = 0; i < fv; i++)
			{
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + i];

				vector3 vertex(attrib.vertices[3 * idx.vertex_index + 0],
					attrib.vertices[3 * idx.vertex_index + 1],
					attrib.vertices[3 * idx.vertex_index + 2]);

				raw_vertices[idx.vertex_index] = vertex;

				if (attrib.normals.size()) {
					vector3 normal(attrib.normals[3 * idx.vertex_index + 0],
						attrib.normals[3 * idx.vertex_index + 1],
						attrib.normals[3 * idx.vertex_index + 2]);

					raw_normals[idx.vertex_index] = normal;
				}

				if (attrib.texcoords.size()) {
					vector2 uv(attrib.texcoords[2 * idx.vertex_index + 0],
						attrib.texcoords[2 * idx.vertex_index + 1]);

					raw_uvs[idx.vertex_index] = uv;
				}

				indices.push_back(idx.vertex_index);
			}

			index_offset += fv;
		}
	}

	int64_t num_triangles = indices.size() / 3;

	//std::cout << "Num triangles = " << this->num_triangles << std::endl;

	std::vector<int> indices_range(num_triangles);
	for (int i = 0; i < num_triangles; i++)
	{
		indices_range[i] = i;
	}

	std::vector<mesh_vertex> mashVertices;
	std::vector<mesh_face> meshFaces;

	mashVertices.clear();
	mashVertices.reserve(raw_vertices.size());

	for (unsigned int i = 0; i < std::max(raw_vertices.size(), raw_normals.size()); i++)
	{
		/*vector3 vertex;
		vector3 normal;

		if (i < raw_vertices.size())
		{
			vertex = raw_vertices[i];
		}

		if (i < raw_normals.size())
		{
			normal = raw_normals[i];
		}

		mashVertices.emplace_back(vertex, normal);*/
	}


	meshFaces.clear();
	//meshFaces.reserve(std::max(indices.size(), raw_uvs.size()) / 3);

	/*for (unsigned int i = 0; i < std::max(v_elements.size(), n_elements.size()); i += 3)
	{
		std::array<int, 3> v = { {0, 0, 0} };
		std::array<int, 3> n = { {-1, -1, -1} };

		if (i + 2 < v_elements.size())
		{
			v[0] = v_elements[i];
			v[1] = v_elements[i + 1];
			v[2] = v_elements[i + 2];
		}

		if (i + 2 < n_elements.size())
		{
			n[0] = n_elements[i];
			n[1] = n_elements[i + 1];
			n[2] = n_elements[i + 2];
		}

		meshFaces.emplace_back(v, n);
	}*/

	mesh.setVertices(mashVertices);
	mesh.setFaces(meshFaces);

	return true;
}