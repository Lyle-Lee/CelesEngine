#pragma once

#include "Core.h"
#include "Timestep.h"
#include "Celes/Events/Event.h"

namespace Celes {

	class Layer
	{
	public:
		CE_API Layer(const std::string& name = "Layer");
		CE_API virtual ~Layer();

		CE_API virtual void OnAttach() {}
		CE_API virtual void OnDetach() {}
		CE_API virtual void OnUpdate(Timestep dTime) {}
		CE_API virtual void OnEvent(Event& e) {}
		CE_API virtual void OnGUIRender() {}

		CE_API inline const std::string& GetName() const { return m_Name; }
	protected:
		std::string m_Name;
	};

}
