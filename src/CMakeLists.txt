set(EXE_NAME shoggoth-demo)

set(KERNEL_LIBRARY_NAME shoggoth-kernel)
set(RENDER_LIBRARY_NAME shoggoth-renderer)
set(RESOURCES_LIBRARY_NAME shoggoth-resources)
set(PHYSICS_LIBRARY_NAME shoggoth-physics)


# Target
set(DEMO_SRC_FILES
    testcomponentfactory.cpp
    testcomponent.cpp
    demo.cpp
    main.cpp
)

set(TARGET_LIBRARIES
    ${KERNEL_LIBRARY_NAME}
    ${RENDER_LIBRARY_NAME}
    ${RESOURCES_LIBRARY_NAME}
    ${PHYSICS_LIBRARY_NAME}
)

add_executable(${EXE_NAME} ${DEMO_SRC_FILES})

# Link libraries
find_package(SDL REQUIRED)

include_directories(${SDL_INCLUDE_DIR})
target_link_libraries(${EXE_NAME} ${TARGET_LIBRARIES})

add_subdirectory(shoggoth-engine)
