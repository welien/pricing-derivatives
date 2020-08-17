# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${ModernCMakeExample_SOURCE_DIR}/include/dataloader/*.hpp")
set(HEADER_LIST "${PricingDerivatives_SOURCE_DIR}/include/dataloader/dataset.hpp"
                "${PricingDerivatives_SOURCE_DIR}/include/strategies/black-scholes.hpp"
                "${PricingDerivatives_SOURCE_DIR}/include/simulation/montecarlo.hpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(dataset dataset.cpp ${HEADER_LIST})
add_library(black-scholes black-scholes.cpp ${HEADER_LIST})
add_library(montecarlo montecarlo.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(dataset PUBLIC ../include)
target_include_directories(black-scholes PUBLIC ../include)
target_include_directories(montecarlo PUBLIC ../include)

# This depends on (header only) boost
target_link_libraries(dataset PRIVATE)
target_link_libraries(black-scholes PRIVATE dataset)
target_link_libraries(montecarlo PRIVATE black-scholes)

# All users of this library will need at least C++11
target_compile_features(dataset PUBLIC cxx_std_11)
target_compile_features(black-scholes PUBLIC cxx_std_11)
target_compile_features(montecarlo PUBLIC cxx_std_11)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
