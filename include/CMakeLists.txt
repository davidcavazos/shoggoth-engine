
# install header files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shoggoth-engine
        DESTINATION ${INSTALL_DIR_INCLUDE}
        FILES_MATCHING PATTERN "*.hpp")
