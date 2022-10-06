#pragma once

#include "Object.h"
#include "Material.h"
#include "FrameBuffer.h"

class DirectionalLight : public Cube
{
public:
	DirectionalLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& upDir, EmissiveMaterial* mtl);
	DirectionalLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& upDir, EmissiveMaterial* mtl, const glm::vec3& _radiance);
	~DirectionalLight();

	glm::mat4 calcLightVP();

	glm::vec3 lightPos, lightDir, lightUp;
	glm::vec3 radiance;
	EmissiveMaterial* material;
	FrameBuffer* fbo = nullptr;
};