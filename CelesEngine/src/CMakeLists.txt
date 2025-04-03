file(GLOB_RECURSE CE_SOURCES
    "Celes/*.cpp"
    "Box2D/src/*.cpp"
    "ImGuizmo/*.cpp"
    "yaml-cpp/src/*.cpp"
    "Platform/OpenGL/*.cpp"
    "Platform/Windows/*.cpp"
)

file(GLOB_RECURSE CE_HEADERS
    "Celes/*.h"
    "Box2D/include/*.h"
    "ImGuizmo/*.h"
    "yaml-cpp/include/*.h"
    "Platform/OpenGL/*.h"
    "Platform/Windows/*.h"
)

add_library(CelesEngine SHARED ${CE_SOURCES} ${CE_HEADERS})

target_precompile_headers(CelesEngine PRIVATE
    PCH.h
)

target_include_directories(CelesEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Box2D/include
    ${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    # $<INSTALL_INTERFACE:include>
)

target_compile_definitions(CelesEngine PRIVATE
    CE_BUILD_DLL
    GLEW_STATIC
    $<$<CONFIG:Debug>:CE_DEBUG>
    $<$<CONFIG:Release>:CE_RELEASE>
)

target_link_libraries(CelesEngine PRIVATE
    OpenGL::GL
    Vulkan::Vulkan
    glfw
    glew_s
)

if(CE_WINDOWS)
    target_sources(CelesEngine PRIVATE
        Platform/Windows/Windows*.cpp
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
