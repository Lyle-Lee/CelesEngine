#include "ContentBrowserPanel.h"
#include <ImGui/imgui.h>

namespace Celes {

	// TODO: change this once we have projects
	extern const std::filesystem::path s_AssetsDirectory = "assets";

	ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory(s_AssetsDirectory)
	{
		m_DirectoryIcon = Texture2D::Create("icons/ContentBrowser/icon-directory.png");
		m_FileIcon = Texture2D::Create("icons/ContentBrowser/icon-file.png");
		m_BackIcon = Texture2D::Create("icons/ContentBrowser/icon-back.png");
	}

	void ContentBrowserPanel::OnGUIRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(s_AssetsDirectory))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(.0f, .0f, .0f, .0f));
			if (ImGui::ImageButton((ImTextureID)m_BackIcon->GetBufferID(), { 25, 25 }, { 0, 1 }, { 1, 0 }))
				m_CurrentDirectory = m_CurrentDirectory.parent_path();

			ImGui::PopStyleColor();
		}

		static float padding = 16.0f, thumbnailSize = 50.0f;
		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCnt = std::max(1, (int)(panelWidth / cellSize));

		ImGui::Columns(columnCnt, 0, false);
		for (auto& dirEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = dirEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetsDirectory);
			std::string fileName = relativePath.filename().string();

			ImGui::PushID(fileName.c_str()); // To make drag and drop events (from image button) to be identical
			Ref<Texture2D> icon = dirEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(.0f, .0f, .0f, .0f));
			ImGui::ImageButton((ImTextureID)icon->GetBufferID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			ImGui::PopID();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (dirEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}
			
			ImGui::TextWrapped(fileName.c_str());
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::End();
	}

}
