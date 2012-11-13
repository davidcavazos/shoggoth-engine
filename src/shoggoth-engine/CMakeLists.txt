
set(KERNEL_SRC_FILES
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
)

set(RENDER_SRC_FILES
    renderer/camera.cpp
    renderer/light.cpp
    renderer/renderablemesh.cpp
    renderer/culling.cpp
    renderer/renderer.cpp
)

set(RESOURCES_SRC_FILES
    resources/texture.cpp
    resources/modelloader.cpp
    resources/material.cpp
    resources/mesh.cpp
    resources/model.cpp
    resources/resources.cpp
)

set(PHYSICS_SRC_FILES
    physics/rigidbody.cpp
    physics/physicsworld.cpp
)

add_library(${KERNEL_LIBRARY_NAME} SHARED ${KERNEL_SRC_FILES})
add_library(${RENDER_LIBRARY_NAME} SHARED ${RENDER_SRC_FILES})
add_library(${RESOURCES_LIBRARY_NAME} SHARED ${RESOURCES_SRC_FILES})
add_library(${PHYSICS_LIBRARY_NAME} SHARED ${PHYSICS_SRC_FILES})

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

target_link_libraries(${KERNEL_LIBRARY_NAME} ${SDL_LIBRARY})
target_link_libraries(${RENDER_LIBRARY_NAME} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${BULLET_LIBRARIES})
target_link_libraries(${RESOURCES_LIBRARY_NAME} ${SDLIMAGE_LIBRARY} ${ASSIMP_LIBRARIES})
target_link_libraries(${PHYSICS_LIBRARY_NAME} ${BULLET_LIBRARIES})


# install libraries
install(TARGETS ${TARGET_LIBRARIES} DESTINATION ${INSTALL_DIR_LIBRARIES})
