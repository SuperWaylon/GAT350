#pragma once
#include "Resources/Resource.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

// forward declare assimp types so the header doesn't pull in assimp
struct aiNode;
struct aiMesh;
struct aiScene;

namespace nu
{
	class Model : public Resource
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec2 texcoord;
			glm::vec3 normal;
			glm::vec3 tangent;
		};

		// each mesh owns its own vertex and index buffer
		struct Mesh
		{
			std::unique_ptr<VertexBuffer> vertexBuffer;
			std::unique_ptr<IndexBuffer> indexBuffer;
		};

	public:
		bool Load(const std::string& filename, const class Renderer& renderer);

		// binds and draws every mesh, call between BeginFrame and EndFrame with a pipeline set
		void Draw(class Renderer& renderer) const;

	private:
		void ProcessNode(aiNode* node, const aiScene* scene, const class Renderer& renderer);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene, const class Renderer& renderer);

	private:
		std::vector<Mesh> m_meshes;
	};
}
