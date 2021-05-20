include_directories(catch2)

set(DEFINE_TEST_HEADER test.cpp)
file(GLOB_RECURSE FILENAMES layouts/*.cpp)

foreach (FILE IN LISTS FILENAMES)
    get_filename_component(NAME ${FILE} NAME_WE)
    set(TEST_NAME test_${NAME})
    add_executable(${TEST_NAME} ${FILE} ${DEFINE_TEST_HEADER})
    target_link_libraries(${TEST_NAME} PRIVATE project_warnings project_options mockturtle)

    add_test(NAME ${NAME} COMMAND ${TEST_NAME})
endforeach ()