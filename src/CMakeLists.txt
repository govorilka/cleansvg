cmake_minimum_required (VERSION 3.9)
project (CleanSVG)
add_executable(CleanSVG main.cpp)

target_link_libraries(CleanSVG SDL2 SDL2Main)

set(EXECUTABLE_OUTPUT_PATH ${CLEANSVG_OUTPUT_DIR})

if(WIN32)
   set_target_properties(CleanSVG PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
   set_target_properties(CleanSVG PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")

   set_target_properties(CleanSVG PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")

   set_target_properties(CleanSVG PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
   set_target_properties(CleanSVG PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")

   set_target_properties(CleanSVG PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS")
endif(WIN32)
