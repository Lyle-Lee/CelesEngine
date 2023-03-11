#pragma once

#include <Celes.h>

struct ParticleInfo
{
	glm::vec2 Position;
	glm::vec2 Velocity, VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd, SizeVariation;
	float LifeTime = 1.0f;
};

class ParticleSystem
{
public:
	ParticleSystem(uint32_t maxCnt = 1000);

	void OnUpdate(Celes::Timestep dTime);
	void OnRender(Celes::OrthoCamera& camera);

	void Emit(const ParticleInfo& info);
private:
	struct Particle
	{
		glm::vec2 Position, Velocity;
		glm::vec4 ColorBegin, ColorEnd;
		float Rotation = 0.0f;
		float SizeBegin, SizeEnd;

		float LifeTime = 1.0f, LifeRemain = 0.0f;

		bool Active = false;
	};
	std::vector<Particle> m_ParticlePool;
	uint32_t m_PoolIndex;
};
