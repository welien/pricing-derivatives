cmake_minimum_required(VERSION 3.16)
project(PricingDerivatives VERSION 1.0
                                DESCRIPTION "Pricing financial derivatives using evolutionary heuristics"
                                LANGUAGES CXX)

add_definitions("-std=c++11")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here

  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

endif()


#add_executable(main main.cpp)
#target_link_libraries(main ${CONAN_LIBS})

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(apps)


#add_custom_target(run
#    COMMAND main
#    DEPENDS main
#    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
#)