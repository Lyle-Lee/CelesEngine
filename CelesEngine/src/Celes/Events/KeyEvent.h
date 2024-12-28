#pragma once

#include "Event.h"

namespace Celes {

	class CE_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }
		EVENT_CLASS_CATEGORY(Keyboard | InputEvent)
	protected:
		KeyEvent(int keycode): m_KeyCode(keycode) {}

		int m_KeyCode;
	};

	class CE_API KeyPressEvent : public KeyEvent
	{
	public:
		KeyPressEvent(int keyCode, bool isRepeat = false)
			: KeyEvent(keyCode), m_IsRepeat(isRepeat) {}

		inline bool IsRepeat() const { return m_IsRepeat; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Key Pressed: " << m_KeyCode << " (is repeat: " << m_IsRepeat << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPress)
	private:
		bool m_IsRepeat;
	};

	class CE_API KeyReleaseEvent : public KeyEvent
	{
	public:
		KeyReleaseEvent(int keyCode): KeyEvent(keyCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Key Released: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyRelease)
	};

	class CE_API KeyTypeEvent : public KeyEvent
	{
	public:
		KeyTypeEvent(int keyCode): KeyEvent(keyCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "Key Typed: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyType)
	};

}
