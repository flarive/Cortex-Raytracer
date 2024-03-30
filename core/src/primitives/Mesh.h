#pragma once

#include "../misc/ray.h"
#include "hittable.h"
#include "../utilities/types.h"
#include "../utilities/math_utils.h"
#include "../utilities/interval.h"
#include "../utilities/types.h"
#include "../misc/hit_record.h"
#include "../aabb.h"

#include <array>
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>


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
	mesh(std::string _name = "Mesh") :
		m_boundingBox({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }),
		m_material(nullptr)
	{
		name = _name;
	}

	explicit mesh(std::vector<mesh_vertex> vertices, std::vector<mesh_face> faces, std::string _name = "Mesh") :
		m_vertices(std::move(vertices)),
		m_faces(std::move(faces)),
		m_boundingBox({ 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }),
		m_material(nullptr)
	{
		name = _name;
		
		updateBoundingBox();
	}

	

	void setVertices(std::vector<mesh_vertex> vertices);


	void setFaces(std::vector<mesh_face> faces);


	void setMaterial(std::shared_ptr<material> mat);

	/// <summary>
	/// Apply a glm transformation to the primitive
	/// Such as translate, rotate, scale...
	/// </summary>
	/// <param name="transformation"></param>
	void applyTransformation(const matrix4& transformation);


	int nbVertices() const;


	int nbFaces() const;


	const std::vector<mesh_vertex>& vertices() const;



	const std::vector<mesh_face>& faces() const;


	const std::shared_ptr<material>& materials() const;


	aabb bounding_box() const override;



	bool hit(const ray& r, interval ray_t, hit_record& rec, int depth) const override;



	/**
	 * \brief Return the transformed vertex v (between 0 and 2) from a face
	 * \param face The index of the face of the mesh
	 * \param v The index of the vertex in the face
	 * \return The 3D coordinates of the vertex
	 */
	vector3 vertex(int face, int v) const;


	/**
		* \brief Return the transformed normal at a vertex v (between 0 and 2) from a face
		* \param face The index of the face of the mesh
		* \param v The index of the vertex in the face
		* \return The 3D coordinates of the normal
		*/
	vector3 normal(int face, int v) const;

	/**
		* \brief Return the transformed normal interpolated with barycentric coordinates from a face
		* \param face The index of the face of the mesh
		* \param u The first barycentric coordinate
		* \param v The second barycentric coordinate
		* \return The 3D coordinates of the normal
		*/
	vector3 normal(int face, double u, double v) const;

	static bool loadMesh(const std::filesystem::path& filename, std::vector<mesh_vertex>& vertices, std::vector<mesh_face>& faces);
	static bool loadMesh(const std::filesystem::path& filename, mesh& mesh);



private:
	/**
	 * \brief Update the internal AABB of the mesh.
	 *        Warning: run this when the mesh is updated.
	 */
	void updateBoundingBox() override;


	static bool rayMeshIntersection(const mesh& mesh, const ray& ray, double minT, hit_record& hit);
	

	static bool rayMeshesIntersection(const std::vector<mesh>& meshes, const ray& ray, double minT,	hit_record& hit);
	

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

	static int indexOfNumberLetter(const std::string& str, int offset);
	static int lastIndexOfNumberLetter(const std::string& str);
	static std::vector<std::string> split(const std::string& s, char delim);

};




//bool loadMeshTinyObj(const std::string& file_name, mesh& mesh)
//{
//	tinyobj::attrib_t attrib;
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> materials;
//	std::string warn;
//	std::string err;
//
//	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_name.c_str());
//
//	if (!warn.empty()) {
//		std::cout << "[tinyobj warning] " << warn << std::endl;
//	}
//
//	if (!err.empty()) {
//		std::cerr << "[tinyobj error] " << err << std::endl;
//	}
//
//	if (!ret)
//	{
//		exit(1);
//	}
//
//	std::vector<vector3> raw_vertices;
//	std::vector<vector2> raw_uvs;
//	std::vector<vector3> raw_normals;
//	std::vector<int> indices;
//
//
//	for (size_t s = 0; s < shapes.size(); s++)
//	{
//		size_t index_offset = 0;
//		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
//		{
//			int fv = shapes[s].mesh.num_face_vertices[f];
//			if (fv != 3)
//			{
//				std::cerr << "Only triangle faces allowed at this point" << std::endl;
//				exit(1);
//			}
//
//			raw_vertices.resize(attrib.vertices.size());
//			raw_normals.resize(attrib.vertices.size());
//			raw_uvs.resize(attrib.vertices.size());
//
//			for (int i = 0; i < fv; i++)
//			{
//				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + i];
//
//				vector3 vertex(attrib.vertices[3 * idx.vertex_index + 0],
//					attrib.vertices[3 * idx.vertex_index + 1],
//					attrib.vertices[3 * idx.vertex_index + 2]);
//
//				raw_vertices[idx.vertex_index] = vertex;
//
//				if (attrib.normals.size()) {
//					vector3 normal(attrib.normals[3 * idx.vertex_index + 0],
//						attrib.normals[3 * idx.vertex_index + 1],
//						attrib.normals[3 * idx.vertex_index + 2]);
//
//					raw_normals[idx.vertex_index] = normal;
//				}
//
//				if (attrib.texcoords.size()) {
//					vector2 uv(attrib.texcoords[2 * idx.vertex_index + 0],
//						attrib.texcoords[2 * idx.vertex_index + 1]);
//
//					raw_uvs[idx.vertex_index] = uv;
//				}
//
//				indices.push_back(idx.vertex_index);
//			}
//
//			index_offset += fv;
//		}
//	}
//
//	int64_t num_triangles = indices.size() / 3;
//
//	//std::cout << "Num triangles = " << this->num_triangles << std::endl;
//
//	std::vector<int> indices_range(num_triangles);
//	for (int i = 0; i < num_triangles; i++)
//	{
//		indices_range[i] = i;
//	}
//
//	std::vector<mesh_vertex> mashVertices;
//	std::vector<mesh_face> meshFaces;
//
//	mashVertices.clear();
//	mashVertices.reserve(raw_vertices.size());
//
//	for (unsigned int i = 0; i < std::max(raw_vertices.size(), raw_normals.size()); i++)
//	{
//		vector3 vertex;
//		vector3 normal;
//
//		if (i < raw_vertices.size())
//		{
//			vertex = raw_vertices[i];
//		}
//
//		if (i < raw_normals.size())
//		{
//			normal = raw_normals[i];
//		}
//
//		mashVertices.emplace_back(vertex, normal);
//	}
//
//
//	meshFaces.clear();
//	meshFaces.reserve(std::max(indices.size(), raw_uvs.size()) / 3);
//
//	for (unsigned int i = 0; i < std::max(v_elements.size(), n_elements.size()); i += 3)
//	{
//		std::array<int, 3> v = { {0, 0, 0} };
//		std::array<int, 3> n = { {-1, -1, -1} };
//
//		if (i + 2 < v_elements.size())
//		{
//			v[0] = v_elements[i];
//			v[1] = v_elements[i + 1];
//			v[2] = v_elements[i + 2];
//		}
//
//		if (i + 2 < n_elements.size())
//		{
//			n[0] = n_elements[i];
//			n[1] = n_elements[i + 1];
//			n[2] = n_elements[i + 2];
//		}
//
//		meshFaces.emplace_back(v, n);
//	}
//
//	mesh.setVertices(mashVertices);
//	mesh.setFaces(meshFaces);
//
//	return true;
//}