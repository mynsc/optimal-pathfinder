    cmake_minimum_required(VERSION 3.28)
    project(OptimalPathfinder LANGUAGES CXX)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

    set(CMAKE_POLICY_VERSION_MINIMUM 3.10)

    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
    FetchContent_MakeAvailable(SFML)

    add_executable(${PROJECT_NAME} src/main.cpp
        src/grafo.cpp
        src/dijkstra.cpp
        src/interfaz.cpp
    )

    target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
    target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics)
    target_include_directories(${PROJECT_NAME} PRIVATE include)

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
    )