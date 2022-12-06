#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include "Shader.h"
#include "Texture.h"

enum class AttribValueType
{
	uniform3f = 0, uniform3fv = 1, uniform4f = 2, uniform4fv = 3,
	uniformMat4 = 4, sampler2D = 5
};

struct Attribute
{
	std::string name;
	std::variant<glm::vec3, glm::vec4, glm::mat4, int> value;
	AttribValueType type;

	Attribute(const std::string& _name, AttribValueType _type): name(_name), type(_type) {}
	Attribute(const std::string& _name, const glm::vec3& _value)
		: name(_name), value(_value), type(AttribValueType::uniform3f)
	{}
	Attribute(const std::string& _name, const glm::vec4& _value)
		: name(_name), value(_value), type(AttribValueType::uniform4f)
	{}
	Attribute(const std::string& _name, const glm::mat4& _value)
		: name(_name), value(_value), type(AttribValueType::uniformMat4)
	{}
	Attribute(const std::string& _name, const int _value, AttribValueType _type = AttribValueType::sampler2D)
		: name(_name), value(_value), type(_type)
	{}
};

enum class MatarialType
{
	Undefined = 0, Phong = 1, SSGI = 2, PRT = 3, PBR = 4, KullaConty = 5
};

class Material
{
public:
	Material() {}
	Material(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~Material();

	template<typename T>
	void setAttribute(const std::string& name, const T& value)
	{
		if (m_AttribCache.find(name) != m_AttribCache.end())
		{
			m_Attribs[m_AttribCache[name]].value = value;
		}
		else
		{
			m_Attribs.emplace_back(name, value);
			m_AttribCache[name] = m_Attribs.size() - 1;
		}
	}

	void bindAttribute();

	MatarialType type = MatarialType::Undefined;
	std::unique_ptr<Shader> shader;
private:
	std::vector<Attribute> m_Attribs;
	std::unordered_map<std::string, unsigned int> m_AttribCache;
};

class EmissiveMaterial : public Material
{
public:
	EmissiveMaterial(const glm::vec3& radiance);
	EmissiveMaterial(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, bool enableShadowMap = true);
	EmissiveMaterial(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const glm::vec3& radiance);
	~EmissiveMaterial();

	bool hasShadowMap;
	std::optional<std::unique_ptr<Texture>> shadowMap;
};

class PhongMaterial : public Material
{
public:
	PhongMaterial(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~PhongMaterial();
};