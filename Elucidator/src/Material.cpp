#include "Material.h"
#include "Utils.h"

Material::Material(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	shader = std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath);
}

Material::~Material()
{
}

void Material::bindAttribute()
{
	shader->bind();
	for (auto& attrib : attribs)
	{
		shader->setUniform3fv(attrib.name, 1, &attrib.value.x);
	}
}

EmissiveMaterial::EmissiveMaterial(const glm::vec3& radiance)
	: hasShadowMap(false)
{
	this->attribs.push_back(Attribute<glm::vec3>("uLightRadiance", radiance, AttribValueType::uniform3f));
}

EmissiveMaterial::EmissiveMaterial(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const glm::vec3& radiance)
	: hasShadowMap(true)
{
	this->shader = std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath);
	this->attribs.push_back(Attribute<glm::vec3>("uLightRadiance", radiance, AttribValueType::uniform3fv));

	shadowMap = std::make_unique<Texture>(RESOLUTION, RESOLUTION);
}

EmissiveMaterial::~EmissiveMaterial()
{
}

PhongMaterial::PhongMaterial(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	this->type = MatarialType::Phong;
	this->shader = std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath);
}

PhongMaterial::~PhongMaterial()
{
}
