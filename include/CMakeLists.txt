add_library(libfiction INTERFACE fiction/algorithms/physical_design/ortho_new.hpp fiction/networks/views/topo_view_input_sort.hpp fiction/networks/views/input_sort_view.hpp fiction/algorithms/network_transformation/fanout_inverter_balancing.hpp fiction/algorithms/network_transformation/fo_inv_balancing.hpp fiction/networks/views/reverse_view.hpp fiction/algorithms/physical_design/ortho_sequential.hpp fiction/networks/sequential_technology_network.h)
target_include_directories(libfiction INTERFACE ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/include/)
target_link_libraries(libfiction INTERFACE mockturtle)

# Add configuration file
configure_file(${PROJECT_SOURCE_DIR}/include/fiction/utils/version_info.hpp.in utils/version_info.hpp)
