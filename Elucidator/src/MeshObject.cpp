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
		mtl->setAttribute("uKa", loader.LoadedMaterials[0].Ka, AttribValueType::uniform3fv);
		mtl->setAttribute("uKd", loader.LoadedMaterials[0].Kd, AttribValueType::uniform3fv);
		mtl->setAttribute("uKs", loader.LoadedMaterials[0].Ks, AttribValueType::uniform3fv);
	}
}

MeshObject::~MeshObject() {}
