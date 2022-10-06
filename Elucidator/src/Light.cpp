#include <gtc/matrix_transform.hpp>
#include "Light.h"
//#include "Utils.h"

DirectionalLight::DirectionalLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& upDir, EmissiveMaterial* mtl)
	: lightPos(pos), lightDir(dir), lightUp(upDir), radiance(1.0f), material(mtl)
{
	if (mtl->hasShadowMap)
	{
		fbo = new FrameBuffer(RESOLUTION, RESOLUTION);
		mtl->shadowMap->get()->bind();
		fbo->bindTexture(*mtl->shadowMap->get());
		fbo->bindRenderBuffer();
		fbo->checkStatus();
		fbo->unbind();
	}
}

DirectionalLight::DirectionalLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& upDir, EmissiveMaterial* mtl, const glm::vec3& _radiance)
	: lightPos(pos), lightDir(dir), lightUp(upDir), radiance(_radiance), material(mtl)
{
	if (mtl->hasShadowMap)
	{
		fbo = new FrameBuffer(RESOLUTION, RESOLUTION);
		mtl->shadowMap->get()->bind();
		fbo->bindTexture(*mtl->shadowMap->get());
		fbo->bindRenderBuffer();
		fbo->checkStatus();
		fbo->unbind();
	}
}

DirectionalLight::~DirectionalLight()
{
	delete fbo;
}

glm::mat4 DirectionalLight::calcLightVP()
{
	//float aspect = WINDOW_HEIGHT / static_cast<float>(WINDOW_WIDTH);
	glm::mat4 view = glm::lookAtRH(lightPos, lightPos - lightDir, lightUp);
	glm::mat4 projection = glm::orthoRH_NO(-10.0f, 10.0f, -10.0f, 10.0f, 1e-2f, 250.0f);

	return projection * view;
}
