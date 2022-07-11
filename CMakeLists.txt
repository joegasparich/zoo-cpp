cmake_minimum_required(VERSION 3.20)
project(Zoo)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
if (${APPLE})
    find_package(GLEW REQUIRED)
    find_package(SDL2 REQUIRED)
endif()

file(GLOB_RECURSE sources CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.tpp"
)

set(HEADER_FILES ${CMAKE_SOURCE_DIR}/src/pch.h)
set(SOURCE_FILES ${sources})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${HEADER_FILES})

# Include
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src/include)
if (${APPLE})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
else()
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/windows)
endif()

# Link
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
if (${APPLE})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY})
    target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
else()
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/windows/x64/SDL2main.lib)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/windows/x64/SDL2.lib)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/windows/x64/glew32.lib)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders)
