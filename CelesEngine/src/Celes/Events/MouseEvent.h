#pragma once

#include "Event.h"

namespace Celes {

	class CE_API MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(float x, float y): m_CoordX(x), m_CoordY(y) {}

		inline float GetX() const { return m_CoordX; }
		inline float GetY() const { return m_CoordY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse Moved: " << m_CoordX << ", " << m_CoordY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMove)
		EVENT_CLASS_CATEGORY(Mouse | InputEvent)
	private:
		float m_CoordX, m_CoordY;
	};

	class CE_API MouseScrollEvent : public Event
	{
	public:
		MouseScrollEvent(float x, float y): m_OffsetX(x), m_OffsetY(y) {}

		inline float GetOffsetX() const { return m_OffsetX; }
		inline float GetOffsetY() const { return m_OffsetY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse Scrolled: " << m_OffsetX << ", " << m_OffsetY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScroll)
		EVENT_CLASS_CATEGORY(Mouse | InputEvent)
	private:
		float m_OffsetX, m_OffsetY;
	};

	class CE_API MouseButtonEvent : public Event
	{
	public:
		inline int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(Mouse | InputEvent)
	protected:
		MouseButtonEvent(int button): m_Button(button) {}

		int m_Button;
	};

	class CE_API MouseButtonPressEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressEvent(int button): MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse Button Pressed: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPress)
	};

	class CE_API MouseButtonReleaseEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleaseEvent(int button): MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Mouse Button Released: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonRelease)
	};

}
