add_executable(Glamoth
    *.h
    *.cpp
    Panels/*.h
    Panels/*.cpp
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
