add_library(libfiction INTERFACE)
target_include_directories(libfiction INTERFACE ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/include/)
target_link_libraries(libfiction INTERFACE mockturtle)

# Add configuration file
configure_file(${PROJECT_SOURCE_DIR}/include/fiction/utils/version_info.hpp.in utils/version_info.hpp)
