# Include alice
add_subdirectory(alice/)

# Include mockturtle
add_subdirectory(mockturtle/)

# Mugen is not available under Windows
if (NOT WIN32)
    # Option to enable Mugen
    option(FICTION_ENABLE_MUGEN "Enable the usage of Mugen, a Python3 library by Winston Haaswijk for FCN one-pass synthesis, and its dependencies" OFF)
    if (FICTION_ENABLE_MUGEN)
        target_compile_definitions(libfiction INTERFACE MUGEN)
    endif ()

    if (FICTION_ENABLE_MUGEN)

        # Apple does not need glucose because it seems to have issues there anyways
        if (NOT APPLE)
            # Build glucose-syrup-4.1-parallel if Mugen is enabled
            message(STATUS "Building glucose for Mugen")
            add_custom_command(
                    OUTPUT ${PROJECT_BINARY_DIR}/glucose-syrup
                    PRE_BUILD
                    COMMAND make
                    COMMAND mv glucose-syrup ${PROJECT_BINARY_DIR}/glucose-syrup
                    COMMAND make clean
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/mugen/glucose-syrup-4.1/parallel/)

            # Make sure glucose's custom build commands are actually being executed
            add_custom_target(glucose_syrup
                    ALL
                    DEPENDS ${PROJECT_BINARY_DIR}/glucose-syrup)
        endif ()

        # Embedding the pybind11 interpreter
        target_link_libraries(libfiction INTERFACE pybind11::embed)

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mugen/mugen_info.hpp.in utils/mugen_info.hpp)
        target_include_directories(libfiction INTERFACE ${PROJECT_BINARY_DIR}/libs/)

        message(STATUS "Mugen was enabled. Please note that it relies on the Python3 libraries 'graphviz', 'PySAT v0.1.6.dev6', and 'wrapt_timeout_decorator' to be properly installed")
    endif ()
endif ()

# Enable the usage of Z3
option(FICTION_Z3 "Find, include, and utilize the Z3 solver by Microsoft Research. It needs to be installed manually.")
if (FICTION_Z3)
    message(STATUS "Usage of the Z3 solver was enabled. Make sure that it is installed on your system!")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/") # include FindZ3.cmake

    # Try to locate Z3 (minimum required version is 4.8.5 due to its performance improvements on the QBF solver)
    find_package(Z3 4.8.5)

    if (Z3_FOUND)
        # Status update
        message(STATUS "Found Z3 solver version: ${Z3_VERSION_STRING}")
        message(STATUS "Found Z3 library: ${Z3_LIBRARIES}")
        message(STATUS "Found Z3 include directories: ${Z3_CXX_INCLUDE_DIRS}")

        # Threads are used by Z3 and are, thus, required at this point
        find_package(Threads REQUIRED)

        # Compile definition to guard include files
        target_compile_definitions(libfiction INTERFACE FICTION_Z3_SOLVER)
        # Include Z3 library
        target_include_directories(libfiction INTERFACE ${Z3_CXX_INCLUDE_DIRS})
        # Link Z3
        target_link_libraries(libfiction INTERFACE ${Z3_LIBRARIES})

        # use libc++ on macOS
        if (APPLE)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
        endif ()

    else ()
        message(SEND_ERROR "Z3 solver could not be detected")
    endif ()

endif ()

# If using GCC, find TBB if installed
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/") # include FindTBB.cmake
    find_package(TBB)
    if (TBB_FOUND)
        # Status update
        message(STATUS "Found TBB version: ${TBB_VERSION_MAJOR}.${TBB_VERSION_MINOR}")
        message(STATUS "Parallel STL algorithms are enabled")

        # Include TBB
        target_include_directories(libfiction INTERFACE ${TBB_INCLUDE_DIRS})
        # Link TBB
        target_link_libraries(libfiction INTERFACE TBB::tbb)
    else ()
        message(STATUS "Parallel STL algorithms are disabled. If you want to use them, please install TBB and set the TBB_ROOT_DIR, TBB_INCLUDE_DIR, and TBB_LIBRARY variables accordingly.")
    endif ()
endif ()
