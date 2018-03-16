cmake_minimum_required(VERSION 3.9)

include(ExternalProject)

#
# SDL2 2.0.8 
#

set(SDL2_VERSION "2.0.8")

if (CMAKE_BUILD_TYPE MATCHES "[dD][eE][bB][uU][gG]")
	set(SDL2_BUILD_TYPE "Debug")
else ()
	set(SDL2_BUILD_TYPE "Release")
endif ()

set(SDL2_SRC "${CMAKE_CURRENT_SOURCE_DIR}/SDL2-${SDL2_VERSION}")
MESSAGE( STATUS "SDL2_SRC: " ${SDL2_SRC} )

if (NOT EXISTS ${SDL2_SRC})
	message(FATAL_ERROR "Can't find SDL2 sources.")
endif ()

# taken from BUILD.gn (skia_public_includes, minus things that are obviously useless for us)
set(SDL2_HEADERS
	${SDL2_SRC}/include
)

add_library(SDL2 STATIC IMPORTED GLOBAL)
set(SDL2_LIBRARY "${SDL2_SRC}/lib/x86/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(SDL2 PROPERTIES
	IMPORTED_LOCATION "${SDL2_LIBRARY}"
	INTERFACE_INCLUDE_DIRECTORIES "${SDL2_HEADERS}"
)

add_library(SDL2Main STATIC IMPORTED GLOBAL)
set(SDL2_MAIN_LIBRARY "${SDL2_SRC}/lib/x86/${CMAKE_STATIC_LIBRARY_PREFIX}SDL2main${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_target_properties(SDL2Main PROPERTIES
	IMPORTED_LOCATION "${SDL2_MAIN_LIBRARY}"
)

configure_file("${SDL2_SRC}/lib/x86/SDL2.dll" "${CLEANSVG_OUTPUT_DIR}/Release/." COPYONLY)
configure_file("${SDL2_SRC}/lib/x86/SDL2.dll" "${CLEANSVG_OUTPUT_DIR}/Debug/." COPYONLY)