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

# Créer l'exécutable
add_executable(${PROJECT_NAME}
    ${SOURCES_ROOT}
    ${SOURCES_SUBSYSTEMS}
)

# Inclure les headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/SubSystems
    ${CMAKE_SOURCE_DIR}/SDL/include
)

# Lier avec SDL3
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)