#pragma once

#include "Celes/Core/Layer.h"
#include "Celes/Events/KeyEvent.h"
#include "Celes/Events/MouseEvent.h"
#include "Celes/Events/ApplicationEvent.h"

namespace Celes {

	class CE_API GUILayer : public Layer
	{
	public:
		GUILayer();
		~GUILayer();

		void OnAttach() override;
		void OnDetach() override;
		//void OnGUIRender() override;
		void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool isEnabled) { m_BlockEvents = isEnabled; }

		void SetDarkThemeColors();
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
