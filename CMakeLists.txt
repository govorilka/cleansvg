cmake_minimum_required(VERSION 3.10)

project(CleanSVG)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(ext/glfw)

include_directories(
    ext/glfw/include/
    ext/glfw/deps/
    ext/glm/
    ext/nanosvg/src/
    ext/nanosvg/example/ 
)

file(GLOB VENDORS_SOURCES ext/glfw/deps/glad.c)
file(GLOB PROJECT_HEADERS src/*.hpp)
file(GLOB PROJECT_SOURCES src/*.cpp)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} 
                               ${PROJECT_HEADERS}
                               ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES}
                               )
target_link_libraries(${PROJECT_NAME} glfw
                      ${GLFW_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

if(MSVC)
   set_target_properties(CleanSVG PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
   set_target_properties(CleanSVG PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")

   set_target_properties(CleanSVG PROPERTIES LINK_FLAGS_RELEASE "/ENTRY:mainCRTStartup /SUBSYSTEM:WINDOWS")
   set_target_properties(CleanSVG PROPERTIES COMPILE_DEFINITIONS_DEBUG "_WINDOWS")

   set_target_properties(CleanSVG PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
   set_target_properties(CleanSVG PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")

   set_target_properties(CleanSVG PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif(MSVC)

if (APPLE)
    find_package(OpenGL REQUIRED)

    target_include_directories(CleanSVG PUBLIC ${OPENGL_INCLUDE_DIR})

    target_link_libraries(CleanSVG "-framework Cocoa")
    target_link_libraries(CleanSVG "-framework OpenGL")
    target_link_libraries(CleanSVG "-framework IOKit")
    target_link_libraries(CleanSVG ${OPENGL_gl_LIBRARY})
endif (APPLE)