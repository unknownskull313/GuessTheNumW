#include "mesh.h"


void Model::Load3DModel(std::string ObjFillPath)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(ObjFillPath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		std::cerr << "Assimp load failed: " << import.GetErrorString() << "\n";
	}
	else
	{
		processNodeM(scene->mRootNode, scene);
	}
}

void Model::processNodeM(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMeshM(mesh, scene);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNodeM(node->mChildren[i], scene);
	}
}

void Model::processMeshM(aiMesh* mesh, const aiScene* scene)
{
	Mesh meshM;
	int Vcount = 0;
	int Icount = 0;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 pos;
		pos.x = mesh->mVertices[i].x;
		pos.y = mesh->mVertices[i].y;
		pos.z = mesh->mVertices[i].z;
		glm::vec2 texCoords = glm::vec2(0, 0);
		glm::vec3 normal = glm::vec3(0, 0, 0);
		if (mesh->HasTextureCoords(0)) // 0 = first UV channel
		{
			aiVector3D tex = mesh->mTextureCoords[0][i];
			texCoords = glm::vec2(tex.x, tex.y);
		}
		if (mesh->HasNormals())
		{
			aiVector3D norm= mesh->mNormals[i];
			normal = glm::vec3(norm.x, norm.y, norm.z);
		}
		meshM.vertices.emplace_back(Vertex(pos, meshM.subMeshCount, texCoords, normal));
		Vcount += 1;
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			meshM.indices.emplace_back(meshM.VBatchOffset + face.mIndices[j]);
			Icount++;
		}
	}
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

		aiString texPath;
		if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
		{
			meshM.TexturePath=texPath.C_Str();
			//meshM.m_material = new Material(texture, 1.0f); // need to be in setup
		}
		else
		{
			std::cout << "texture is nullptr somewhere \n";
			meshM.m_material = new Material(nullptr, 1.0f);
		}
	}
	meshM.subMeshIDMap[meshM.subMeshCount] = glm::vec4(meshM.VBatchOffset, meshM.VBatchOffset + Vcount, meshM.IBatchOffset, meshM.IBatchOffset + Icount);
	meshM.subMeshModelMatMap.emplace_back(glm::mat4(1.0f));
	meshM.VBatchOffset += Vcount;
	meshM.IBatchOffset += Icount;
	meshM.subMeshCount += 1;

	meshM.IsLoaded = true;

	meshes.emplace_back(meshM);
}

void Model::setupModel()
{
	for (auto& mesh : meshes) 
	{
		mesh.setupMesh();
	}
}

void Model::Draw(Shader& shader)
{
	for (auto& mesh : meshes)
	{
		shader.SetMat4("ModelMat", mesh.localTransform);
		mesh.Draw(shader);
	}
}

void Mesh::Draw(Shader& shader)
{
	if (CanDraw)
	{
		shader.SetMat4("ModelMat", glm::mat4(1.0f));
		m_material->Bind(shader);
		VAO->Bind();
		VB->Bind();
		IB->Bind();
		SSBO->Bind();
		SSBO->SetBaseBinding(0);
		glDrawElementsBaseVertex(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, 0);
	}
}

void Mesh::DrawLines(Shader& shader)
{
	shader.SetMat4("ModelMat", glm::mat4(1.0f));
	m_material->Bind(shader);
	VAO->Bind();
	VB->Bind();
	IB->Bind();
	SSBO->Bind();
	SSBO->SetBaseBinding(0);
	glDrawElementsBaseVertex(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0, 0);
}

void Mesh::SetSubMeshIdentityMat()
{
	for (auto& submeshmat : subMeshModelMatMap)
	{
		submeshmat = glm::mat4(1.0f);
	}
}

void Mesh::Load3DObj(std::string ObjFillPath)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(ObjFillPath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		std::cerr << "Assimp load failed: " << import.GetErrorString() << "\n";
	}
	else
	{
		this->IsLoaded = true;
		processNode(scene->mRootNode, scene);
	}
}

void Mesh::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void Mesh::processMesh(aiMesh* mesh, const aiScene* scene)
{
	int Vcount = 0;
	int Icount = 0;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		glm::vec3 pos;
		pos.x = mesh->mVertices[i].x;
		pos.y = mesh->mVertices[i].y;
		pos.z = mesh->mVertices[i].z;
		glm::vec2 texCoords = glm::vec2(0, 0);
		glm::vec3 normal = glm::vec3(0, 0, 0);
		if (mesh->HasTextureCoords(0)) // 0 = first UV channel
		{
			aiVector3D tex = mesh->mTextureCoords[0][i];
			texCoords = glm::vec2(tex.x, tex.y);
		}
		if (mesh->HasNormals())
		{
			aiVector3D norm = mesh->mNormals[i];
			normal = glm::vec3(norm.x, norm.y, norm.z);
		}
		this->vertices.emplace_back(Vertex(pos, this->subMeshCount, texCoords, normal));
		Vcount+=1;
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{ 
		this->indices.emplace_back(this->VBatchOffset + face.mIndices[j]);
		Icount++;
		}
	}
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

		aiString texPath;
		if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
		{
			Texture* texture = new Texture(texPath.C_Str());//TODO see Model and upgrade this
			this->m_material = new Material(texture, 1.0f); // default color multiplier
			std::cout << "Loading :" << texPath.C_Str() << "\n";
		}
		else
		{
			std::cout << "texture nullptr at :" << texPath.C_Str() << "\n";
			this->m_material = new Material(nullptr, 1.0f); // fallback material with no texture
		}
	}
	subMeshIDMap[subMeshCount] = glm::vec4(VBatchOffset, VBatchOffset + Vcount, IBatchOffset, IBatchOffset + Icount);
	subMeshModelMatMap.emplace_back(glm::mat4(1.0f));
	this->VBatchOffset += Vcount;
	this->IBatchOffset += Icount;
	subMeshCount += 1;
}


void Mesh::setupMesh()
{
	if (this->IsLoaded)
	{
		Texture* tex = new Texture(TexturePath);
		std::cout << "Loading texture at :" << TexturePath << "\n";
		tex->Upload();
		this->m_material = new Material(tex,1.0f);
		this->VAO = new VertexArray;
		this->VAO->Bind();
		this->VB = new VertexBuffer;
		this->VB->Bind();
		this->VB->SubData(0, vertices.size() * sizeof(Vertex), vertices.data());

		this->VAO->EnableIndex(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		this->VAO->EnableIndex(1);
		glVertexAttribIPointer(1, 1, GL_INT, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
		this->VAO->EnableIndex(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(int)));
		this->VAO->EnableIndex(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(int) + sizeof(glm::vec2)));



		this->IB = new IndexBuffer;
		this->IB->SubData(0, indices.size() * sizeof(unsigned int), indices.data());
		this->SSBO = new ShaderStorageBuffer;
		this->SSBO->SetBaseBinding(0);
		this->SSBO->SubData(0, subMeshModelMatMap.size() * sizeof(glm::mat4), subMeshModelMatMap.data());

		this->VAO->Unbind();
		this->VB->Unbind();
		this->IB->Unbind();
		this->SSBO->Unbind();

		this->CanDraw = true;
	}
}


BatchedModel::BatchedModel()
{
}

BatchedModel::BatchedModel(Mesh& mesh)
{
}

void BatchedModel::AppendMesh(Mesh& mesh)
{
	meshes.emplace_back(mesh);
}

void BatchedModel::AppendMeshes(std::vector<Mesh> also_meshes)
{
	for (auto& mesh : also_meshes)
	{
		meshes.emplace_back(mesh);
	}
}

void BatchedModel::AccessMeshSubMeshMat(int mesh_index, int mat_index, glm::mat4 modelmat)
{
	//iam also changing the RAM ; then, when i want to sub dynamicaly i need to change this , (just del below)
	meshes[mesh_index].subMeshModelMatMap[mat_index] = modelmat;
	meshes[mesh_index].SSBO->SubData(sizeof(glm::mat4)*mat_index , sizeof(glm::mat4) , &modelmat);
}

void BatchedModel::SetTransforMesh(glm::mat4 model, int index)
{
	meshes[index].localTransform = model;
}

void BatchedModel::Draw(Shader& shader)
{
	for (auto& mesh : meshes)
	{
		shader.SetMat4("ModelMat", mesh.localTransform);
		mesh.Draw(shader);
		shader.SetMat4("ModelMat", glm::mat4(1.0f));
	}
}