set(EXE_NAME shoggoth-demo)

# Target
set(DEMO_SRC_FILES
    testcomponentfactory.cpp
    testcomponent.cpp
    demo.cpp
    main.cpp
)

add_executable(${EXE_NAME} ${DEMO_SRC_FILES})

# Link libraries
find_package(SDL REQUIRED)

include_directories(${SDL_INCLUDE_DIR})
target_link_libraries(${EXE_NAME} shoggoth-engine)

add_subdirectory(shoggoth-engine)
