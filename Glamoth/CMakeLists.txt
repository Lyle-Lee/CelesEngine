set(GLAMOTH_ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(GLAMOTH_ICONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/icons)

add_executable(Glamoth
    src/EditorLayer.h
    src/EditorLayer.cpp
    src/Glamoth.cpp
    src/Panels/ContentBrowserPanel.h
    src/Panels/ContentBrowserPanel.cpp
    src/Panels/SceneHierarchyPanel.h
    src/Panels/SceneHierarchyPanel.cpp
)

target_link_libraries(Glamoth PRIVATE
    CelesEngine
)

if(UNIX AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_command(TARGET Glamoth POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${GLAMOTH_ASSETS_DIR}
            $<TARGET_FILE_DIR:Glamoth>/assets
    )

    add_custom_command(TARGET Glamoth POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${GLAMOTH_ICONS_DIR}
            $<TARGET_FILE_DIR:Glamoth>/icons
    )
else()
    add_custom_command(TARGET Glamoth POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${GLAMOTH_ASSETS_DIR}
            $<TARGET_FILE_DIR:Glamoth>/assets
    )

    add_custom_command(TARGET Glamoth POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${GLAMOTH_ICONS_DIR}
            $<TARGET_FILE_DIR:Glamoth>/icons
    )
endif()

if(CE_WINDOWS)
    add_custom_command(TARGET Glamoth POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:CelesEngine>
            $<TARGET_FILE_DIR:Glamoth>
    )
endif()
