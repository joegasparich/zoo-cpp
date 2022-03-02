cmake_minimum_required(VERSION 3.20)
project(test)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
#find_package(SDL2_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost 1.78.0)


add_executable(test src/main.cpp src/Game.cpp src/Game.h src/Entity.cpp src/Entity.h src/Stage.h src/constants/assets.h src/AssetManager.cpp src/AssetManager.h src/Mediator.cpp src/Mediator.h src/constants/events.h src/SceneManager.cpp src/SceneManager.h src/Scene.h src/InputManager.cpp src/InputManager.h src/Stage.cpp src/util/util.h src/components/Component.h src/components/RenderComponent.cpp src/components/RenderComponent.h src/components/Component.cpp src/util/typeMap.h src/util/math.h src/renderer/VertexBuffer.cpp src/renderer/VertexBuffer.h src/renderer/IndexBuffer.cpp src/renderer/IndexBuffer.h src/renderer/VertexArray.cpp src/renderer/VertexArray.h src/renderer/VertexBufferLayout.h src/renderer/Shader.cpp src/renderer/Shader.h src/renderer/Renderer.cpp src/renderer/Renderer.h src/renderer/Texture.cpp src/renderer/Texture.h src/lib/stb_image/stb_image.cpp src/lib/stb_image/stb_image.h)

target_include_directories(test PUBLIC ${SDL2_INCLUDE_DIRS})
#target_include_directories(test PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})
target_include_directories(test PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(test PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(test ${SDL2_LIBRARY})
#target_link_libraries(test ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(test ${OPENGL_LIBRARIES})
target_link_libraries(test GLEW::GLEW)

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/img ${CMAKE_BINARY_DIR}/img)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders)
