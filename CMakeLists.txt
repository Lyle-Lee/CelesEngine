cmake_minimum_required(VERSION 3.16)

cmake_policy(SET CMP0077 NEW)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0104 NEW)
cmake_policy(SET CMP0057 NEW)

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

project(CelesEngine
    DESCRIPTION "Celes Engine"
    LANGUAGES CXX C
    VERSION 0.1.0
)

# -----------------------------------------------------------------------------
# Configuration options
# -----------------------------------------------------------------------------

# Enable/disable asserts. AUTO enables asserts in debug builds.
set(CE_ENABLE_ASSERTS "AUTO" CACHE STRING "Enable asserts")
set_property(CACHE CE_ENABLE_ASSERTS PROPERTY STRINGS AUTO ON OFF)

# Enable/disable the profiler.
set(CE_ENABLE_PROFILER ON CACHE BOOL "Enable profiler")

# Enable/disable using system Python distribution. This requires Python 3.7 to be available.
set(CE_USE_SYSTEM_PYTHON OFF CACHE BOOL "Use system Python distribution")

# Enable/disable USD.
# set(CE_ENABLE_USD ON CACHE BOOL "Enable USD")


# Enable/disable Address Sanitizer.
set(CE_ENABLE_ASAN OFF CACHE BOOL "Enable Address Sanitizer")

# Header validation.
# If enabled, additional targets are generated to validate that headers are self sufficient.
set(CE_VALIDATE_HEADERS OFF CACHE BOOL "Enable header validation")

# Precompiled headers.
# If enabled, precompiled headers are used to speed up compilation.
set(CE_PRECOMPILED_HEADERS ON CACHE BOOL "Enable precompiled headers")

# -----------------------------------------------------------------------------
# Check platform
# -----------------------------------------------------------------------------

if(${CMAKE_SYSTEM_NAME} MATCHES "Window")
    set(CE_PLATFORM "Windows")
    set(CE_WINDOWS TRUE)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CE_PLATFORM "Linux")
    set(CE_LINUX TRUE)
else()
    message(CE_ERROR "Unsupported platform!")
endif()
message(STATUS "Platform: ${CE_PLATFORM}")

# Check if D3D12/Vulkan are available.
set(CE_HAS_D3D12 ${CE_WINDOWS})
set(CE_HAS_VULKAN ON)

# -----------------------------------------------------------------------------
# git submodules
# -----------------------------------------------------------------------------

# if(NOT EXISTS ${CMAKE_SOURCE_DIR}/external/pybind11/CMakeLists.txt)
#     message(STATUS "Updating git submodules ...")
#     find_package(Git REQUIRED)
#     execute_process(
#         COMMAND ${GIT_EXECUTABLE} submodule update --init
#         COMMAND_ERROR_IS_FATAL ANY
#     )
# endif()

# -----------------------------------------------------------------------------
# Misc
# -----------------------------------------------------------------------------

# Setup workaround for re-running mt.exe when it fails due to a race condition with AV software.
# include(mt-retry)

# Embed git information.
# include(git_version)
# git_version_setup()

# On Linux, we build with RPATH set to $ORIGIN to make build relocatable.
# Also, we link with precompiled libraries that are compiled with _GLIBCXX_USE_CXX11_ABI=0,
# so we need to compile with the same flag to avoid ABI incompatibilities.
# if(CE_LINUX)
#     set(CMAKE_INSTALL_RPATH $ORIGIN)
#     set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
#     set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0)
# endif()


# -----------------------------------------------------------------------------
# CUDA
# -----------------------------------------------------------------------------

# Set default CUDA architecture to 75 (Turing).
# This can be overwritten for individual targets using the CUDA_ARCHITECTURE property.
# if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
#   set(CMAKE_CUDA_ARCHITECTURES 75-virtual)
# endif()

# include(CheckLanguage)
# check_language(CUDA)
# if(CMAKE_CUDA_COMPILER)
#     message(STATUS "Enabling CUDA support.")
#     enable_language(CUDA)
#     find_package(CUDAToolkit REQUIRED)
#     set(CE_HAS_CUDA ON)
# else()
#     set(CE_HAS_CUDA OFF)
# endif()

# -----------------------------------------------------------------------------
# Python
# -----------------------------------------------------------------------------

# if(CE_USE_SYSTEM_PYTHON)
#     find_package(Python COMPONENTS Interpreter Development REQUIRED)
# else()
#     set(Python_ROOT_DIR ${CMAKE_SOURCE_DIR}/external/python)
#     find_package(Python COMPONENTS Interpreter Development REQUIRED)
# endif()

# if(CE_WINDOWS)
#     set(TOOLS_PYTHON ${CMAKE_SOURCE_DIR}/tools/python/python)
# elseif(CE_LINUX)
#     set(TOOLS_PYTHON ${Python_EXECUTABLE})
# endif()

# -----------------------------------------------------------------------------
# Global setup
# -----------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "_d")

# Require builds to be outside of source tree.
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please use a build directory instead.")
endif()

# Enable folders (for Visual Studio).
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Setup available build configurations.
if(NOT SETUP_CONFIGURATION_TYPES)
    set(SETUP_CONFIGURATION_TYPES 1)

    if(CMAKE_CONFIGURATION_TYPES)
        # multi config generator
        set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
    else()
        # single config generator
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release")
    endif()
endif()

set(CE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(CMAKE_CONFIGURATION_TYPES)
    set(CE_OUTPUT_DIRECTORY ${CE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>)
else()
    set(CE_OUTPUT_DIRECTORY ${CE_RUNTIME_OUTPUT_DIRECTORY})
endif()

set(CE_SHADER_OUTPUT_DIRECTORY ${CE_OUTPUT_DIRECTORY}/shaders)
set(CE_PLUGIN_OUTPUT_DIRECTORY ${CE_OUTPUT_DIRECTORY}/plugins)

# -----------------------------------------------------------------------------
# External dependencies
# -----------------------------------------------------------------------------

find_package(OpenGL REQUIRED)
find_package(Vulkan REQUIRED)

add_subdirectory(CelesEngine/Dependencies)

message(STATUS "Feature flags:")
message(STATUS "CE_HAS_D3D12: ${CE_HAS_D3D12}")
message(STATUS "CE_HAS_VULKAN: ${CE_HAS_VULKAN}")
# message(STATUS "CE_HAS_AFTERMATH: ${CE_HAS_AFTERMATH}")
# message(STATUS "CE_HAS_NVAPI: ${CE_HAS_NVAPI}")
# message(STATUS "CE_HAS_PIX: ${CE_HAS_PIX}")
# message(STATUS "CE_HAS_CUDA: ${CE_HAS_CUDA}")
# message(STATUS "CE_HAS_OPTIX: ${CE_HAS_OPTIX}")
# message(STATUS "CE_HAS_D3D12_AGILITY_SDK: ${CE_HAS_D3D12_AGILITY_SDK}")
# message(STATUS "CE_HAS_NRD: ${CE_HAS_NRD}")
# message(STATUS "CE_HAS_DLSS: ${CE_HAS_DLSS}")
# message(STATUS "CE_HAS_NV_USD: ${CE_HAS_NV_USD}")
# message(STATUS "CE_HAS_MDL_SDK: ${CE_HAS_MDL_SDK}")
# message(STATUS "CE_ENABLE_USD: ${CE_ENABLE_USD}")

# -----------------------------------------------------------------------------
# Shader file handling
# -----------------------------------------------------------------------------

# Regex for matching shader files.
set(SHADER_EXTENSION_REGEX "\.(slang|slangh|hlsli|hlsl)$")

# Setup build rules to copy all shaders of a target to the output directory.
# The specified output_dir is relative to the global shader output directory (CE_SHADER_OUTPUT_DIRECTORY).
function(target_copy_shaders target output_dir)
    get_target_property(target_source_dir ${target} SOURCE_DIR)
    set(shader_output_dir ${CE_SHADER_OUTPUT_DIRECTORY}/${output_dir})

    # Get list of all target sources.
    get_target_property(target_sources_ ${target} SOURCES)

    # Create custom commands for copying shader sources.
    foreach(file ${target_sources_})
        if(${file} MATCHES ${SHADER_EXTENSION_REGEX})
            if(IS_ABSOLUTE ${file})
                file(RELATIVE_PATH file ${target_source_dir} ${file})
            endif()

            set(src_file ${target_source_dir}/${file})
            set(dst_file ${shader_output_dir}/${file})

            add_custom_command(
                OUTPUT ${dst_file}
                COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${src_file} ${dst_file}
                MAIN_DEPENDENCY ${src_file}
                COMMENT "${target}: Copying shader ${file}"
            )
        endif()
    endforeach()
endfunction()

# -----------------------------------------------------------------------------
# Data file handling
# -----------------------------------------------------------------------------

# add_custom_target(copy_data_folder ALL
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CE_OUTPUT_DIRECTORY}/data
# )
# set_target_properties(copy_data_folder PROPERTIES FOLDER "Misc")

# add_custom_target(copy_scripts_folder ALL
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scripts ${CE_OUTPUT_DIRECTORY}/scripts
# )
# set_target_properties(copy_scripts_folder PROPERTIES FOLDER "Misc")

# -----------------------------------------------------------------------------
# Celes applications
# -----------------------------------------------------------------------------

# Create a Celes application and link the main Celes library.
function(add_ce_executable target)
    add_executable(${target})

    target_link_libraries(${target} PRIVATE CelesEngine)

    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CE_RUNTIME_OUTPUT_DIRECTORY})

    set_target_properties(${target} PROPERTIES VS_DEBUGGER_ENVIRONMENT "CE_DEVMODE=1")
endfunction()

# -----------------------------------------------------------------------------
# Plugins
# -----------------------------------------------------------------------------

# Global render pass target list.
set_property(GLOBAL PROPERTY CE_PLUGIN_TARGETS)

# Create a Celes plugin.
function(add_plugin target)
    add_library(${target} SHARED)

    target_link_libraries(${target} PRIVATE CelesEngine)

    set_target_properties(${target} PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY ${CE_PLUGIN_OUTPUT_DIRECTORY}
        LIBRARY_OUTPUT_DIRECTORY ${CE_PLUGIN_OUTPUT_DIRECTORY}
        INSTALL_RPATH "\$ORIGIN/../"
    )

    # Add target to global plugin target list.
    set_property(GLOBAL APPEND PROPERTY CE_PLUGIN_TARGETS ${target})
endfunction()

# -----------------------------------------------------------------------------
# Helpers
# -----------------------------------------------------------------------------

# Helper function to create a source group for Visual Studio.
# This adds all the target's sources to a source group in the given folder.
function(target_source_group target folder)
    set_target_properties(${target} PROPERTIES FOLDER ${folder})
    get_target_property(target_source_dir ${target} SOURCE_DIR)
    get_target_property(target_sources_ ${target} SOURCES)
    source_group(TREE ${target_source_dir} FILES ${target_sources_} PREFIX "")
endfunction()

# -----------------------------------------------------------------------------
# Header validation
# -----------------------------------------------------------------------------

# Helper function to validate header files of a target to be self sufficient.
# This creates a new target with "VH" suffix that compiles one
# translation unit for each header file found in the supplied targets sources list.
function(validate_headers target)
    if(NOT CE_VALIDATE_HEADERS)
        return()
    endif()

    cmake_parse_arguments(PARSE_ARGV 1 validate_header_args "" "" "IGNORE")
    set(ignore_files ${validate_header_args_IGNORE})

    message(STATUS "Setting up header validation for target: ${target}")

    # Create a new target for validating headers.
    set(validate_target ${target}VH)
    add_library(${validate_target} OBJECT)

    # Copy properties from original target.
    foreach(property INCLUDE_DIRECTORIES LINK_LIBRARIES COMPILE_DEFINITIONS COMPILE_FEATURES)
        get_target_property(value ${target} ${property})
        if(value)
            set_property(TARGET ${validate_target} PROPERTY ${property} ${value})
        endif()
    endforeach()

    # Add target source directory as an include directory.
    get_target_property(target_source_dir ${target} SOURCE_DIR)
    target_include_directories(${validate_target} PRIVATE ${target_source_dir})

    # Get list of source files.
    get_target_property(sources ${target} SOURCES)

    # Create a list of CPP files, each including one header.
    set(validate_sources "")
    foreach(file ${sources})
        if(${file} MATCHES "^[^.][^.].*\\.h$" AND NOT ${file} IN_LIST ignore_files)
            string(SHA1 id ${file})
            # shorten to help avoid super long file names
            string(SUBSTRING ${id} 0, 8, id)
            set(cpp_file ${validate_target}/${id}.cpp)
            set(cpp_content "#include \"${file}\"\n")
            file(CONFIGURE OUTPUT ${cpp_file} CONTENT ${cpp_content})
            list(APPEND validate_sources ${CMAKE_CURRENT_BINARY_DIR}/${cpp_file})
        endif()
    endforeach()

    target_compile_options(${validate_target} PRIVATE $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/bigobj>)

    target_sources(${validate_target} PRIVATE ${validate_sources})
    set_target_properties(${validate_target} PROPERTIES FOLDER "Validation")
endfunction()

# -----------------------------------------------------------------------------
# Ninja log analyzer
# -----------------------------------------------------------------------------

# if(${CMAKE_GENERATOR} MATCHES "Ninja")
#     add_custom_target(ninja_log_analyzer ALL
#         COMMAND ${TOOLS_PYTHON} ${CMAKE_SOURCE_DIR}/tools/ninja_log_analyzer.py ${CMAKE_CURRENT_BINARY_DIR}/.ninja_log -c 10
#     )
#     add_dependencies(ninja_log_analyzer CelesEngine CelesPython CelesTest Glamoth ${plugin_targets})
#     set_target_properties(ninja_log_analyzer PROPERTIES FOLDER "Misc")
# endif()

# -----------------------------------------------------------------------------
# Project sources
# -----------------------------------------------------------------------------

add_subdirectory(CelesEngine/src)
add_subdirectory(Glamoth/src)
# add_subdirectory(Sandbox/src)

# add_dependencies(CelesEngine copy_data_folder copy_scripts_folder)

# Make core library depend on deploying all dependencies.
add_dependencies(CelesEngine deploy_dependencies)

# Get list of all plugin targets.
get_property(plugin_targets GLOBAL PROPERTY CE_PLUGIN_TARGETS)

# Generate plugins.json file.
if(plugin_targets)
    set(json ${plugin_targets})
    list(TRANSFORM json PREPEND "\"")
    list(TRANSFORM json APPEND "\"")
    list(JOIN json ", " json)
    string(PREPEND json "[ ")
    string(APPEND json " ]")
    file(GENERATE OUTPUT ${CE_PLUGIN_OUTPUT_DIRECTORY}/plugins.json CONTENT ${json})
endif()

# Generate settings.toml file.
file(GENERATE OUTPUT ${CE_OUTPUT_DIRECTORY}/settings.json CONTENT "{ \"standardsearchpath\" : { \"media\" : \"\${CE_MEDIA_FOLDERS}\", \"mdl\" : \"\${CE_MDL_PATHS}\" }}")

# Make Glamoth and CelesPython depend on all plugins.
if(plugin_targets)
    add_dependencies(Glamoth ${plugin_targets})
    # add_dependencies(CelesPython ${plugin_targets})
    # add_dependencies(Glamoth CelesPython)
endif()

# Make Mogwai the default startup project in VS.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Glamoth)
