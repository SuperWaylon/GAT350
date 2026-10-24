#include "pch.h"
#include "Model.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

namespace nu
{
	bool Model::Load(const std::string& filename, const Renderer& renderer)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename,
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Could not load assimp file " << filename << ": " << importer.GetErrorString() << std::endl;

			return false;
		}

		// start processing the scene from the root node
		ProcessNode(scene->mRootNode, scene, renderer);

		return true;
	}

	void Model::Draw(Renderer& renderer) const
	{
		for (const auto& mesh : m_meshes)
		{
			renderer.SetVertexBuffer(*mesh.vertexBuffer);
			renderer.SetIndexBuffer(*mesh.indexBuffer);
			renderer.DrawIndex(*mesh.indexBuffer);
		}
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene, const Renderer& renderer)
	{
		// process the current node meshes
		// a node only stores mesh indices, the actual meshes are stored in scene->mMeshes
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			ProcessMesh(mesh, scene, renderer);
		}

		// process the current node children (recursive)
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene, renderer);
		}
	}

	void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const Renderer& renderer)
	{
		// get model vertex attributes
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;

			vertex.position = glm::vec3{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

			// texcoords are optional, use (0, 0) if the mesh doesn't have them
			vertex.texcoord = (mesh->mTextureCoords[0])
				? glm::vec2{ mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y }
			: glm::vec2{ 0, 0 };

			// normals are optional, use (0, 0, 0) if the mesh doesn't have them
			vertex.normal = (mesh->mNormals)
				? glm::vec3{ mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z }
			: glm::vec3{ 0, 0, 0 };

			// tangents are optional, use (0, 0, 0) if the mesh doesn't have them
			vertex.tangent = (mesh->mTangents)
				? glm::vec3{ mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z }
			: glm::vec3{ 0, 0, 0 };

			vertices.push_back(vertex);
		}

		// get model indices
		// each face is a triangle (aiProcess_Triangulate), each face stores the indices of its vertices
		std::vector<uint32_t> indices;
		indices.reserve(mesh->mNumFaces * 3);

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		SDL_GPUDevice* gpuDevice = renderer.GetGPUDevice();

		// create the vertex buffer (uploads to the gpu)
		Mesh newMesh;
		newMesh.vertexBuffer = std::make_unique<VertexBuffer>();
		if (!newMesh.vertexBuffer->Create(vertices, gpuDevice))
		{
			std::cerr << "Could not create vertex buffer for mesh " << mesh->mName.C_Str() << std::endl;

			return;
		}

		// create the index buffer (uploads to the gpu)
		newMesh.indexBuffer = std::make_unique<IndexBuffer>();
		if (!newMesh.indexBuffer->Create(indices, gpuDevice))
		{
			std::cerr << "Could not create index buffer for mesh " << mesh->mName.C_Str() << std::endl;

			return;
		}

		// Mesh holds unique_ptrs so it can't be copied, move it
		m_meshes.push_back(std::move(newMesh));
	}
}