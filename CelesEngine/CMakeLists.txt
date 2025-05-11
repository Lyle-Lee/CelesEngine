file(GLOB_RECURSE CE_SOURCES
    src/Celes/*.cpp
    src/Box2D/src/*.cpp
    src/ImGui/*.cpp
    src/ImGuizmo/*.cpp
    src/yaml-cpp/src/*.cpp
    src/Platform/OpenGL/*.cpp
    src/Platform/Windows/*.cpp
)

file(GLOB_RECURSE CE_HEADERS
    src/Celes/*.h
    src/Box2D/include/*.h
    src/ImGui/*.h
    src/ImGuizmo/*.h
    src/yaml-cpp/include/*.h
    src/Platform/OpenGL/*.h
    src/Platform/Windows/*.h
)

add_library(CelesEngine SHARED ${CE_SOURCES} ${CE_HEADERS})

target_precompile_headers(CelesEngine PRIVATE
    src/PCH.h
)

target_include_directories(CelesEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SPDLOG_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${ENTT_INCLUDE_DIR}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(CelesEngine PRIVATE
    ${GLFW_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${Vulkan_INCLUDE_DIRS}
    ${STBIMAGE_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Box2D/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/yaml-cpp/include
)

target_compile_definitions(CelesEngine PRIVATE
    CE_BUILD_DLL
    GLEW_STATIC
    $<$<CONFIG:Debug>:CE_DEBUG>
    $<$<CONFIG:Release>:CE_RELEASE>
)

target_link_libraries(CelesEngine PRIVATE
    ${GLFW_LIBRARY}
    ${GLEW_LIBRARY}
    OpenGL::GL
    Vulkan::Vulkan
    $<$<CONFIG:Debug>:SPIRV-Toolsd.lib>
    $<$<CONFIG:Debug>:shaderc_sharedd.lib>
    $<$<CONFIG:Release>:shaderc_shared.lib>
    $<$<CONFIG:Debug>:spirv-cross-cored.lib>
    $<$<CONFIG:Release>:spirv-cross-core.lib>
    $<$<CONFIG:Debug>:spirv-cross-glsld.lib>
    $<$<CONFIG:Release>:spirv-cross-glsl.lib>
)

if(CE_WINDOWS)
    target_sources(CelesEngine PRIVATE
        src/Platform/Windows/WindowsInput.cpp
        src/Platform/Windows/WindowsUtils.cpp
        src/Platform/Windows/WindowsWindow.h
        src/Platform/Windows/WindowsWindow.cpp
    )
    
    target_compile_definitions(CelesEngine PRIVATE
        NOMINMAX
        _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
    )
endif()

install(TARGETS CelesEngine
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

# install(DIRECTORY include/ DESTINATION include)
