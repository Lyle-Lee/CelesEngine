#pragma once

#include "Core.h"
#include "Layer.h"
#include <vector>

namespace Celes {

	class CE_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void Push(Layer* layer);
		void PushOverlay(Layer* layer);
		void Pop(Layer* layer);
		void PopOverlay(Layer* layer);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	private:
		std::vector<Layer*> m_Layers;
		//std::vector<Layer*>::iterator m_Top;
		unsigned int m_InsertIdx = 0;
	};

}
