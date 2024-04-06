#pragma once

#include <Celes.h>
#include <filesystem>

namespace Celes {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnGUIRender();
	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};

}
