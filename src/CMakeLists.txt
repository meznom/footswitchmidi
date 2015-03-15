set (CMAKE_VERBOSE_MAKEFILE on)

find_package (PkgConfig)

pkg_check_modules (JACK REQUIRED jack)
include_directories (${JACK_INCLUDE_DIRS})
link_directories (${JACK_LIBRARY_DIRS})

pkg_check_modules (LIBUSB REQUIRED libusb-1.0)
include_directories (${LIBUSB_INCLUDE_DIRS})
link_directories (${LIBUSB_LIBRARY_DIRS})

pkg_check_modules (LIBLO REQUIRED liblo)
include_directories (${LIBLO_INCLUDE_DIRS})
link_directories (${LIBLO_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -m64 -O0 -Wall -Wno-unused-local-typedefs -march=native -g")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -m64 -O3 -Wall -Wno-unused-local-typedefs -march=native -DNDEBUG")

# make sure that the default is a Release
if (NOT CMAKE_BUILD_TYPE)
    set (
        CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
        FORCE
    )
endif (NOT CMAKE_BUILD_TYPE)

add_executable (footswitchmidi main.cpp)
target_link_libraries (footswitchmidi ${JACK_LIBRARIES})
target_link_libraries (footswitchmidi ${LIBUSB_LIBRARIES})
target_link_libraries (footswitchmidi ${LIBLO_LIBRARIES})

install(TARGETS footswitchmidi RUNTIME DESTINATION bin)
install(CODE "
EXECUTE_PROCESS(COMMAND ln -sf footswitchmidi footswitchosc
   WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin
   )
")
