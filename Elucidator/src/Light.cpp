#include <gtc/matrix_transform.hpp>
#include "Light.h"
//#include "Utils.h"

DirectionalLight::DirectionalLight(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& _upDir, EmissiveMaterial* mtl)
	: lightPos(pos), lightDir(dir), upDir(_upDir), material(mtl)
{
	radiance = material->attribs[0].value;

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
	glm::mat4 view = glm::lookAtRH(lightPos, lightPos + lightDir, upDir);
	glm::mat4 projection = glm::orthoRH_NO(-10.0f, 10.0f, -10.0f, 10.0f, 1e-2f, 250.0f);

	return projection * view;
}
