include_directories(catch2 .)

set(DEFINE_TEST_HEADER test.cpp algorithms/physical_design/ortho_new.cpp networks/views/topo_view_input_sort.cpp networks/views/input_sort_view.cpp algorithms/network_transformation/fanout_inverter_balancing.cpp algorithms/network_transformation/fo_inv_balancing.cpp networks/views/reverse_view.cpp algorithms/physical_design/ortho_sequential.cpp algorithms/physical_design/ortho_ordering_network.cpp algorithms/physical_design/ortho_majority_gates_network.cpp algorithms/physical_design/ortho_sequential_network.cpp)
file(GLOB_RECURSE FILENAMES */*.cpp)
list(FILTER SOURCES EXCLUDE REGEX "catch2/*")

foreach (FILE IN LISTS FILENAMES)
    get_filename_component(NAME ${FILE} NAME_WE)
    set(TEST_NAME test_${NAME})
    add_executable(${TEST_NAME} ${FILE} ${DEFINE_TEST_HEADER})
    target_compile_definitions(${TEST_NAME} INTERFACE CATCH_CONFIG_NO_POSIX_SIGNALS) # make catch2 ignore SIGTERMs sent to applications when timeouts are reached
    target_link_libraries(${TEST_NAME} PRIVATE project_warnings project_options libfiction)

    add_test(NAME ${NAME} COMMAND ${TEST_NAME})
endforeach ()
