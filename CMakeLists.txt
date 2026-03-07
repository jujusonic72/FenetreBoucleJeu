cmake_minimum_required(VERSION 3.16)
project(MonMoteur LANGUAGES CXX)

# Standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dossier de sortie pour l'exécutable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/x64/Debug)

# Ajouter SDL3 comme sous-projet
add_subdirectory(SDL)

# Collecter tes fichiers sources
file(GLOB SOURCES_ROOT "*.cpp")
file(GLOB SOURCES_SUBSYSTEMS "SubSystems/*.cpp")
file(GLOB SOURCES_COMPONENTS "Components/*.cpp")
file(GLOB SOURCES_SCENES "Scenes/*.cpp")
file(GLOB SOURCES_IMGUI "imgui/*.cpp")
file(GLOB SOURCES_ENTITIES "Entities/*.cpp")
# Créer l'exécutable
add_executable(${PROJECT_NAME}
    ${SOURCES_ROOT}
    ${SOURCES_SUBSYSTEMS}
    ${SOURCES_IMGUI}
    ${SOURCES_COMPONENTS}
    ${SOURCES_SCENES}
    ${SOURCES_ENTITIES}
)

# Inclure les headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/SubSystems
    ${CMAKE_SOURCE_DIR}/SDL/include
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/Components
    ${CMAKE_SOURCE_DIR}/Scenes
    ${CMAKE_SOURCE_DIR}/Entities
)

# Trouver et lier OpenGL
find_package(OpenGL REQUIRED)

# Lier avec SDL3 et OpenGL
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 OpenGL::GL)
