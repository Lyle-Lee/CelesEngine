#pragma once

#include "Celes/Core/Layer.h"
#include "Celes/Events/KeyEvent.h"
#include "Celes/Events/MouseEvent.h"
#include "Celes/Events/ApplicationEvent.h"

namespace Celes {

	class GUILayer : public Layer
	{
	public:
		CE_API GUILayer();
		CE_API ~GUILayer();

		CE_API void OnAttach() override;
		CE_API void OnDetach() override;
		//void OnGUIRender() override;
		CE_API void OnEvent(Event& e) override;

		CE_API void Begin();
		CE_API void End();

		CE_API void BlockEvents(bool isEnabled) { m_BlockEvents = isEnabled; }

		CE_API void SetDarkThemeColors();
	private:
		/*bool OnMouseButtonPressEvent(MouseButtonPressEvent& e);
		bool OnMouseButtonReleaseEvent(MouseButtonReleaseEvent& e);
		bool OnMouseMoveEvent(MouseMoveEvent& e);
		bool OnMouseScrollEvent(MouseScrollEvent& e);
		bool OnKeyPressEvent(KeyPressEvent& e);
		bool OnKeyReleaseEvent(KeyReleaseEvent& e);
		bool OnKeyTypeEvent(KeyTypeEvent& e);
		bool OnWindowResizeEvent(WindowResizeEvent& e);*/

		float m_Time = 0.0f;
		bool m_BlockEvents = true;
	};

}
