#pragma once

#include "Core.h"
#include "Timestep.h"
#include "Celes/Events/Event.h"

namespace Celes {

	class CE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep dTime) {}
		virtual void OnEvent(Event& e) {}
		virtual void OnGUIRender() {}

		inline const std::string& GetName() const { return m_Name; }
	protected:
		std::string m_Name;
	};

}
