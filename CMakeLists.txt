cmake_minimum_required(VERSION 3.28)

project(VULKAN_Renderer
    VERSION 0.1.0
    DESCRIPTION "Vulkan Renderer"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed. Use: cmake -B build")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

include(FetchContent)

# --------------------------------------------------------------------------------------
# Vulkan
# --------------------------------------------------------------------------------------
find_package(Vulkan REQUIRED)

# --- GLFW (either use system package or fetch with CMake) ---
option(USE_SYSTEM_GLFW "Use system-installed GLFW" OFF)

if(USE_SYSTEM_GLFW)
    find_package(glfw3 REQUIRED)
else()
    include(FetchContent)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
    )
    FetchContent_MakeAvailable(glfw)
endif()

# --------------------------------------------------------------------------------------
# glm (header-only math library)
# --------------------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# --------------------------------------------------------------------------------------
# tinyobjloader
# --------------------------------------------------------------------------------------

FetchContent_Declare(
    tinyobjloader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader
    GIT_TAG v2.0.0rc5
)

FetchContent_MakeAvailable(tinyobjloader)

# --------------------------------------------------------------------------------------
# Native File Dialog Extended (nfd)
# --------------------------------------------------------------------------------------

set(NFD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(NFD_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    nfd
    GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
    GIT_TAG v1.3.0
)

FetchContent_GetProperties(nfd)
if(NOT nfd_POPULATED)
    FetchContent_MakeAvailable(nfd)
endif()

# --------------------------------------------------------------------------------------
# Dear ImGui core
# --------------------------------------------------------------------------------------

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.1
)

FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_MakeAvailable(imgui)
endif()

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# --------------------------------------------------------------------------------------
# Renderer executable
# --------------------------------------------------------------------------------------

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

file(GLOB_RECURSE SHADER_FILES
	"${CMAKE_SOURCE_DIR}/shaders/*"
)
source_group("Shaders" FILES ${SHADER_FILES})

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${SHADER_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    tinyobjloader
    imgui
    nfd
    Vulkan::Vulkan
    glfw
    glm
)

if(UNIX AND NOT APPLE)
    # Required by some GLFW setups
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE X11)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Organize files in VS filters using the src tree
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${PROJECT_SOURCES} ${PROJECT_HEADERS})

# Expose project version to C++ code as APP_VERSION
target_compile_definitions(${PROJECT_NAME} PRIVATE
    APP_VERSION=\"${PROJECT_VERSION}\"
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(MSVC)
    # Run with working directory set to the folder containing the .exe
    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif()

# For release builds on MSVC, build as a windowed app (no console)
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
endif()

# Set startup project for Visual Studio
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# --- Shader compilation ---

set(SHADER_DIR      ${CMAKE_SOURCE_DIR}/shaders)
set(SHADER_OUT_DIR  ${CMAKE_BINARY_DIR}/shaders)

file(MAKE_DIRECTORY ${SHADER_OUT_DIR})

# Gather all shader sources
file(GLOB SHADER_SOURCES
    "${SHADER_DIR}/*.vert"
    "${SHADER_DIR}/*.frag"
)

set(COMPILED_SHADERS "")

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV "${SHADER_OUT_DIR}/${FILE_NAME}.spv")

    add_custom_command(
        OUTPUT  ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUT_DIR}
        COMMAND glslc ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader: ${FILE_NAME}"
        VERBATIM
    )

    list(APPEND COMPILED_SHADERS ${SPIRV})
endforeach()

# Group shader compilation into one target
add_custom_target(compile_shaders DEPENDS ${COMPILED_SHADERS})

# Make sure your executable waits for shaders to be compiled
add_dependencies(${PROJECT_NAME} compile_shaders)

# --- Copy shaders next to the executable after it is built ---
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${SHADER_OUT_DIR} "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMENT "Copying shaders to runtime directory"
)


# set the source directory of resources
set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)

# Copy the contents of the res folder to the build directory if there is one
if(EXISTS ${RES_DIR})
  message(STATUS "Resource folder fond: ${RES_DIR}")
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RES_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying resources to the build directory..."
  )
else()
  # Skipping copying resources
  message(STATUS "Resource folder not found: ${RES_DIR}. Skipping sesource copy.")
endif()
