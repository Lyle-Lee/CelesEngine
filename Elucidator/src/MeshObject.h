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
	MeshObject(const std::string& filepath);
	MeshObject(const std::string& filepath, Material* mtl);
	~MeshObject();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material* material;
};