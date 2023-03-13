#include "ParticleSystem.h"
#include <random>
#include <gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/compatibility.hpp>

class Random
{
public:
	static void Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}

	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
	}
private:
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};

std::mt19937 Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

ParticleSystem::ParticleSystem(uint32_t maxCnt): m_PoolIndex(maxCnt - 1)
{
	m_ParticlePool.resize(maxCnt);
}

void ParticleSystem::OnUpdate(Celes::Timestep dTime)
{
	for (auto& ptc : m_ParticlePool)
	{
		if (!ptc.Active) continue;

		if (ptc.LifeRemain <= 0.0f)
		{
			ptc.Active = false;
			continue;
		}

		ptc.LifeRemain -= dTime;
		ptc.Position += ptc.Velocity * (float)dTime;
		ptc.Rotation += 0.01f * dTime;
	}
}

void ParticleSystem::OnRender(Celes::OrthoCamera& camera)
{
	Celes::Renderer2D::BeginScene(camera);
	for (auto& ptc : m_ParticlePool)
	{
		if (!ptc.Active) continue;

		// Fade away particles
		float life = ptc.LifeRemain / ptc.LifeTime;
		glm::vec4 color = glm::lerp(ptc.ColorEnd, ptc.ColorBegin, life);
		//color.a *= life;

		float size = glm::lerp(ptc.SizeEnd, ptc.SizeBegin, life);

		Celes::Renderer2D::DrawRotatedQuad({ ptc.Position.x, ptc.Position.y, 0.2f }, { size, size }, ptc.Rotation, color);
	}
	Celes::Renderer2D::EndScene();
}

void ParticleSystem::Emit(const ParticleInfo& info)
{
	Particle& ptc = m_ParticlePool[m_PoolIndex];
	ptc.Active = true;
	ptc.Position = info.Position;
	ptc.Rotation = Random::Float() * 2.0f * glm::pi<float>();

	// Velocity
	ptc.Velocity = info.Velocity;
	ptc.Velocity.x += info.VelocityVariation.x * (Random::Float() - 0.5f);
	ptc.Velocity.y += info.VelocityVariation.y * (Random::Float() - 0.5f);

	// Color
	ptc.ColorBegin = info.ColorBegin;
	ptc.ColorEnd = info.ColorEnd;

	ptc.LifeTime = info.LifeTime;
	ptc.LifeRemain = info.LifeTime;
	ptc.SizeBegin = info.SizeBegin * info.SizeVariation * (Random::Float() - 0.5f);
	ptc.SizeEnd = info.SizeEnd;

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}
