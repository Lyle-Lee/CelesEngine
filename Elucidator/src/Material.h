#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "Shader.h"
#include "Texture.h"

enum class AttribValueType
{
	uniform3f = 0, uniform3fv = 1, uniform4f = 2, uniform4fv = 3,
	uniformMat4 = 4, sampler2D = 5
};

template<typename T>
struct Attribute
{
	std::string name;
	T value;
	AttribValueType type;

	Attribute(const std::string& _name, AttribValueType _type): name(_name), type(_type) {}
	Attribute(const std::string& _name, const T& _value, AttribValueType _type)
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
	void setAttribute(const std::string& name, const T& value, AttribValueType type)
	{
		attribs.push_back(Attribute<T>(name, value, type));
	}

	void bindAttribute();

	MatarialType type = MatarialType::Undefined;
	std::unique_ptr<Shader> shader;
	std::vector<Attribute<glm::vec3>> attribs;
};

class EmissiveMaterial : public Material
{
public:
	EmissiveMaterial(const glm::vec3& radiance);
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