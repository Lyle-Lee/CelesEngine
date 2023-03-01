#include <PCH.h>
#include "LayerStack.h"

namespace Celes {

	LayerStack::LayerStack()
	{
		//m_Top = m_Layers.begin();
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers) delete layer;
	}

	void LayerStack::Push(Layer* layer)
	{
		//m_Top = m_Layers.emplace(m_Top, layer);
		m_Layers.emplace(m_Layers.begin() + m_InsertIdx, layer);
		++m_InsertIdx;
	}

	void LayerStack::PushOverlay(Layer* layer)
	{
		m_Layers.emplace_back(layer);
	}

	void LayerStack::Pop(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			//--m_Top;
			--m_InsertIdx;
		}
	}

	void LayerStack::PopOverlay(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end()) m_Layers.erase(it);
	}

}
