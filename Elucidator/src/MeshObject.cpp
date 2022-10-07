#include "MeshObject.h"
#include "Utils.h"
#include "loaders/OBJLoader.h"

MeshObject::MeshObject(const std::string& filepath, Material* mtl)
	: material(mtl)
{
	loader::OBJLoader loader(vertices, indices);
	ASSERT(loader.loadFile(filepath))

	if (loader.LoadedMaterials.size() > 0)
	{
		m_Kd = loader.LoadedMaterials[0].Kd;
	}
}

MeshObject::MeshObject(const std::string& filepath, Material* mtl, Texture* tex, unsigned int slot)
	: material(mtl), m_Kd(*tex), m_TextureSlot(slot)
{
	loader::OBJLoader loader(vertices, indices);
	ASSERT(loader.loadFile(filepath))
}

MeshObject::~MeshObject() {}

void MeshObject::bindTexture()
{
	if (m_TextureSlot >= 0)
	{
		std::get<Texture>(m_Kd).bind(m_TextureSlot);
		material->shader->setUniform1i("uKd", m_TextureSlot);
		//material->setAttribute<int>("uKd", m_TextureSlot);
	}
	else
	{
		material->setAttribute<glm::vec3>("uKd", std::get<glm::vec3>(m_Kd));
	}
}
