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
	for (auto& attrib : m_Attribs)
	{
		switch (attrib.type)
		{
		case AttribValueType::uniform3f:
			shader->setUniform3fv(attrib.name, 1, &std::get<glm::vec3>(attrib.value).x);
			break;
		case AttribValueType::uniform4f:
			shader->setUniform4fv(attrib.name, 1, &std::get<glm::vec4>(attrib.value).x);
			break;
		case AttribValueType::uniformMat4:
			shader->setUniformMat4f(attrib.name, std::get<glm::mat4>(attrib.value));
			break;
		case AttribValueType::sampler2D:
			shader->setUniform1i(attrib.name, std::get<int>(attrib.value));
			break;
		default:
			ASSERT(false);
		}
	}
}

EmissiveMaterial::EmissiveMaterial(const glm::vec3& radiance)
	: hasShadowMap(false)
{
	this->setAttribute<glm::vec3>("uLightRadiance", radiance);
}

EmissiveMaterial::EmissiveMaterial(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, bool enableShadowMap)
	: hasShadowMap(enableShadowMap)
{
	this->shader = std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath);

	if (enableShadowMap)
		shadowMap = std::make_unique<Texture>(RESOLUTION, RESOLUTION);
}

EmissiveMaterial::EmissiveMaterial(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const glm::vec3& radiance)
	: hasShadowMap(false)
{
	this->shader = std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath);
	this->setAttribute<glm::vec3>("uLightRadiance", radiance);

	//shadowMap = std::make_unique<Texture>(RESOLUTION, RESOLUTION);
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
