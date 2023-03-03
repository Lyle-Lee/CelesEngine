#pragma once

#include <Celes.h>

class Sandbox2D : public Celes::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	void OnAttach() override;
	void OnDetach()	override;
	void OnUpdate(Celes::Timestep dTime) override;
	void OnEvent(Celes::Event& e) override;
	void OnGUIRender() override;
private:
	Celes::ShaderLibrary m_ShaderLib;

	Celes::Ref<Celes::VertexArray> m_VertexArray;
	Celes::Ref<Celes::Texture2D> m_Texture;
	
	Celes::OrthoCameraController m_CameraController;

	glm::vec3 m_ObjPos;
	float m_ObjMoveSpeed = 1.0f;
	glm::vec3 m_ObjColor = { 0.2f, 0.5f, 0.9f };
};
