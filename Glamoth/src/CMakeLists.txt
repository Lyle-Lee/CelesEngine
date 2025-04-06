add_executable(Glamoth
    EditorLayer.h
    EditorLayer.cpp
    Glamoth.cpp
    Panels/ContentBrowserPanel.h
    Panels/ContentBrowserPanel.cpp
    Panels/SceneHierarchyPanel.h
    Panels/SceneHierarchyPanel.cpp
)

target_link_libraries(Glamoth PRIVATE
    CelesEngine
    glfw
)

if(CE_WINDOWS)
    add_custom_command(TARGET Glamoth POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:CelesEngine>
            $<TARGET_FILE_DIR:Glamoth>
    )
endif()
