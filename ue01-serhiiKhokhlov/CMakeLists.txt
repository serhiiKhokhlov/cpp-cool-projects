cmake_minimum_required(VERSION 3.21)
project(games)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) # enable virtual folders for projects if supported by the build system/IDE

# set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# configure C17
set(CMAKE_C_STANDARD 17)          # request C17
set(CMAKE_C_STANDARD_REQUIRED ON) # enforce requested standard
set(CMAKE_C_EXTENSIONS OFF)       # disable compiler specific extensions

# configure C++23
set(CMAKE_CXX_STANDARD 23)          # request C++23
set(CMAKE_CXX_STANDARD_REQUIRED ON) # enforce requested standard
set(CMAKE_CXX_EXTENSIONS OFF)       # disable compiler specific extensions

find_package(SFML COMPONENTS System Window Graphics Audio Network CONFIG REQUIRED)

# helper function to simplify definition of projects
function(target name)
	file(GLOB_RECURSE SRC "src/${name}/src/*")   # recursively collect all files in sub-folder for project
	file(GLOB_RECURSE RESOURCE_FILES "src/${name}/resources/*")
	
	add_executable(${name} ${SRC}   "src/breakout/src/simple_game_object.h"  "src/breakout/src/event_listener.h" "src/breakout/src/event.h" "src/breakout/src/engines.h"  "src/breakout/src/game_engine.h"  "src/breakout/src/controller.h" "src/breakout/src/custom-classes/breakout_game_objs.h"  )       # define project "name" based on files found in SRC

	#set required warning flags
	if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU"        OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"        OR
	   "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang"      OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"      OR
	   "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang"   )
		target_compile_options(${name} PRIVATE
			-Wall        # enable "all" warnings
			-Wextra      # enable extra warnings
			-Wpedantic   # enable strict conformance warnings
			-Wconversion # enable warnings for dangerous implicit conversions
			-Werror=vla  # disable support for VLAs
		)
		target_link_libraries(${name} PRIVATE m)
	elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		target_compile_options(${name} PRIVATE
			/W4          # enable almost all "necessary" warnings
			/permissive- # enforce strict standard compliance
			/JMC         # optimize debugging experience
			/MP          # enable parallel compilation
		)
	else()
		message(WARNING "unknown compiler, no warning flags set!")
	endif()

	# link sfml libraries to all targets
	target_link_libraries(${name} PRIVATE SFML::System SFML::Network SFML::Graphics SFML::Window SFML::Audio) 

	# copy resources to output directory
	if (RESOURCE_FILES)
		add_custom_command(
			TARGET ${name} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${RESOURCE_FILES} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/)
	endif()
endfunction()

target(intro)
target(breakout)