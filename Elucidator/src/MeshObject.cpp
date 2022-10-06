#include "MeshObject.h"
#include "Utils.h"
#include "loaders/OBJLoader.h"

MeshObject::MeshObject(const std::string& filepath)
	: material(nullptr)
{
	loader::OBJLoader loader(vertices, indices);
	ASSERT(loader.loadFile(filepath))
}

MeshObject::MeshObject(const std::string& filepath, Material* mtl)
	: material(mtl)
{
	loader::OBJLoader loader(vertices, indices);
	ASSERT(loader.loadFile(filepath))

	if (loader.LoadedMaterials.size() > 0)
	{
		mtl->setAttribute<glm::vec3>("uKa", loader.LoadedMaterials[0].Ka);
		mtl->setAttribute<glm::vec3>("uKd", loader.LoadedMaterials[0].Kd);
		mtl->setAttribute<glm::vec3>("uKs", loader.LoadedMaterials[0].Ks);
	}
}

MeshObject::~MeshObject() {}
