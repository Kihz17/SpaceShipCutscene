#pragma once

#include "pch.h"
#include "AABB.h"
#include "Texture.h"
#include "VertexInformation.h"
#include "VertexArrayObject.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <unordered_map>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoord;
	glm::vec3 tangent;
	glm::vec3 binormal;
};

struct Face
{
	uint32_t v1, v2, v3;
};

//struct Submesh
//{
//	Submesh() : boundingBox(glm::vec3(0.0f), glm::vec3(0.0f)) {}
//
//	uint32_t baseVertex;
//	uint32_t baseIndex;
//	uint32_t materialIndex;
//	uint32_t indexCount;
//	uint32_t vertexCount;
//
//	AABB boundingBox;
//
//	glm::mat4 transform{ 1.0f };
//	std::string nodeName;
//	std::string meshName;
//};

struct Submesh
{
	Submesh() : boundingBox(glm::vec3(0.0f), glm::vec3(0.0f)) {}

	Ref<VertexArrayObject> vertexArray;
	Ref<VertexBuffer> vertexBuffer;
	Ref<IndexBuffer> indexBuffer;

	std::vector<Vertex> vertices;
	std::vector<Face> faces;

	AABB boundingBox;

	glm::mat4 transform{ 1.0f };
	std::string nodeName;
	std::string meshName;
};

class Mesh
{
public:
	Mesh(const std::string& filePath);
	Mesh(const Ref<Mesh> mesh);
	virtual ~Mesh();
	
	inline std::unordered_map<std::string, Submesh>& GetSubmeshes() { return this->submeshes; }
	inline const std::unordered_map<std::string, Submesh>& GetSubmeshes() const { return this->submeshes; }
	void AddSubmesh(const std::string& filePath);

	const BufferLayout& GetVertexBufferLayout() const { return this->bufferLayout; }

	std::vector<Vertex> GetVertices() const;
	std::vector<Face> GetFaces() const;

	inline AABB& GetBoundingBox() { return this->boundingBox; }
	inline const AABB& GetBoundingBoxSafe() const { return this->boundingBox; }

	inline const std::string& GetPath() const { return this->filePath; }

private:
	void SetupMaterials();
	void LoadNodes(aiNode* node, std::vector<std::string>& submeshes, const glm::mat4& parentTransform = glm::mat4(1.0f));
	Submesh LoadSubmesh(aiMesh* assimpMesh);

	Scope<Assimp::Importer> importer;

	std::unordered_map<std::string, Submesh> submeshes;

	glm::mat4 transform;
	glm::mat4 inverseTransform;

	BufferLayout bufferLayout;

	std::unordered_map<aiNode*, std::vector<uint32_t>> nodeMap;
	const aiScene* assimpScene;

	AABB boundingBox;

	std::string filePath;
};