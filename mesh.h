#pragma once
#include <GLEW/glew.h>
#include <vector>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Buffers/VertexBuffer.h"
#include "../Buffers/IndexBuffer.h"
#include "../Buffers/VertexArrya.h"
#include "../Buffers/ShaderStorageBuffer.h"
#include "../Materials/Material.h"


struct Vertex
{
	glm::vec3 position;
	int ID=0;
	glm::vec2 texCoord;
	glm::vec3 normal = glm::vec3(0);

	Vertex() = default;
	Vertex(glm::vec3 pos, int ID, glm::vec2 texcoord , glm::vec3 Normal) 
	{
		this->position = pos;
		this->ID = ID;
		this->texCoord = texcoord; 
		this->normal = Normal;
	};
	~Vertex() = default;
};

struct Mesh
{
	int VBatchOffset=0;
	int IBatchOffset=0;
	int subMeshCount=0;

	bool IsLoaded = false;
	bool CanDraw = false;
	int meshBaseID = 0;
	std::string name = "undefined";

	VertexArray* VAO = nullptr;
	VertexBuffer* VB = nullptr;
	IndexBuffer* IB = nullptr;

	glm::mat4 localTransform = glm::mat4(1.0f);
	ShaderStorageBuffer* SSBO = nullptr;
	
	std::vector<Vertex>vertices;
	std::vector<unsigned int>indices;
	std::string TexturePath = "undefined";
	Material* m_material = nullptr; 

	std::unordered_map<int, glm::vec4>subMeshIDMap;
	std::vector<glm::mat4>subMeshModelMatMap;

	Mesh() = default;
	~Mesh() = default;	

	void Load3DObj(std::string ObjFillPath);
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);
	void setupMesh();
	void Draw(Shader& shader);
	void DrawLines(Shader& shader);

	void SetMeshBaseID(int id) { this->meshBaseID = id; };
	int GetMeshBaseID() { return meshBaseID; };

	void SetName(const std::string& name) { this->name = name; };
	std::string GetName() { return name; };

	void SetTransform(glm::mat4 model) { this->localTransform = model; };

	void SetSubMeshIdentityMat();
};

class Model
{
private:
	std::vector<Mesh>meshes;
public:
	Model() = default;
	~Model() = default;

	void Load3DModel(std::string ObjFillPath);
	void processNodeM(aiNode* node, const aiScene* scene);
	void processMeshM(aiMesh* mesh, const aiScene* scene);
	void setupModel();
	void Draw(Shader& shader);

	int GetMeshCount() { return meshes.size(); };
};

class BatchedModel
{
private:
	std::vector<Mesh>meshes;
	std::string name = "undefined";
public:
	BatchedModel();
	BatchedModel(Mesh& mesh);
	~BatchedModel() = default;

	void AppendMesh(Mesh& mesh);
	void AppendMeshes(std::vector<Mesh> also_meshes) ;

	void AccessMeshSubMeshMat(int mesh_index, int mat_index, glm::mat4 modelmat);
	void SetTransforMesh(glm::mat4 model , int index);
	void Draw(Shader& shader);	

	int GetMeshCount() { return meshes.size(); };
};