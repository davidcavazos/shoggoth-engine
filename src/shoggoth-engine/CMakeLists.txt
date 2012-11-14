
set(ENGINE_SRC_FILES
    kernel/tokentable.cpp
    kernel/commandobject.cpp
    kernel/command.cpp
    kernel/terminal.cpp

    kernel/entity.cpp
    kernel/component.cpp
    kernel/componentfactory.cpp
    kernel/scene.cpp

    kernel/inputs.cpp
    kernel/device.cpp

    renderer/camera.cpp
    renderer/light.cpp
    renderer/renderablemesh.cpp
    renderer/culling.cpp
    renderer/renderer.cpp

    resources/texture.cpp
    resources/modelloader.cpp
    resources/material.cpp
    resources/mesh.cpp
    resources/model.cpp
    resources/resources.cpp

    physics/rigidbody.cpp
    physics/physicsworld.cpp
)

add_library(${LIBRARY_NAME} SHARED ${ENGINE_SRC_FILES})

# Link libraries
find_package(Boost REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Bullet REQUIRED)
find_package(ASSIMP REQUIRED)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${SDL_INCLUDE_DIR}
    ${SDLIMAGE_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${BULLET_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

target_link_libraries(${LIBRARY_NAME}
    ${SDL_LIBRARY}
    ${SDLIMAGE_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${BULLET_LIBRARIES}
    ${ASSIMP_LIBRARIES}
)

# install libraries
install(TARGETS ${LIBRARY_NAME} DESTINATION ${INSTALL_DIR_LIBRARIES})
