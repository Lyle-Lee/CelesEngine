#pragma once

#include <vector>
#include <string>
#include "Object.h"
#include "Material.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

class MeshObject : public Object
{
public:
	MeshObject(const std::string& filepath, Material* mtl = nullptr);
	MeshObject(const std::string& filepath, Material* mtl, Texture* tex, unsigned int slot = 0);
	~MeshObject();

	void bindTexture();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material* material;
private:
	std::variant<glm::vec3, Texture> m_Kd;
	int m_TextureSlot = -1;
};