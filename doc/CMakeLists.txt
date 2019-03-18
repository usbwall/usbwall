find_package(Doxygen)

if (DOXYGEN_FOUND)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY)
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM)
else (NOT(DOXYGEN_FOUND))
  add_custom_target(doc
    COMMAND false
    COMMENT "Missing Doxygen to build doc !")
endif(DOXYGEN_FOUND)
