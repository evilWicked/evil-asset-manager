cmake_minimum_required(VERSION 2.8)

#remove the build types we dont support -before we call project which locks it in
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_INSTALL_PREFIX  "${CMAKE_SOURCE_DIR}" CACHE INTERNAL "NOT USED")

project(evil-asset-manager-examples)
set(EVIL_NAME evil-asset-manager)

if(NOT CMAKE_MODULE_PATH)
	set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
endif()
include(EvilMacros)

evil_setup_environment()

#grab a hook to the repo root directory
get_filename_component(EVIL_REPO_DIR "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)


## LOCATION OF THE EVIL INCLUDE DIRECTORY  

## THIS IS PROBABLY WHAT YOU ARE LOOKING FOR

#the EVIL_REPO_DIR is where the source originally installed.
#If you have followed the instructions then you have may 
#moved the evil directory into your source tree.

#if so then you will need to adjust this so that EVIL_PARENT points
#to the directory above the new location of the evil dir.  This will be used
#to set the visual studio include paths

#remember we are doing it this way so that evil is required to be prepended to 
#your include path to avoid collisions with things such as signal.h that may exist
#on your machine

set(EVIL_PARENT ${EVIL_REPO_DIR})

#eg on windows: set(EVIL_PARENT "C:\\Users\\yourname\\Documents\\some\\path\\")

set(EVIL_DIR ${EVIL_PARENT}/evil)


#the various libraries
set(EVIL_ASSET_DIR ${EVIL_DIR}/asset-manager)
set(EVIL_ASSET_IMP_DIR ${EVIL_ASSET_DIR}/imp) 

set(EVIL_SIGNALS_DIR ${EVIL_DIR}/signals)
set(EVIL_SIGNALS_IMP_DIR ${EVIL_SIGNALS_DIR}/imp) 

set(EVIL_TEST_RENDERER_DIR ${EVIL_DIR}/test-renderer)
set(EVIL_TEST_RENDERER_IMP_DIR ${EVIL_TEST_RENDERER_DIR}/imp) 

set(EVIL_TEST_DIR ${EVIL_DIR}/test-harness)
set(EVIL_TEST_IMP_DIR ${EVIL_TEST_DIR}/imp) 

set(EVIL_THREAD_DIR ${EVIL_DIR}/thread)
set(EVIL_THREAD_IMP_DIR ${EVIL_THREAD_DIR}/imp) 

set(EVIL_UTIL_DIR ${EVIL_DIR}/util)
set(EVIL_UTIL_IMP_DIR ${EVIL_UTIL_DIR}/imp) 

set(EVIL_MATH_DIR ${EVIL_DIR}/math)
set(EVIL_MATH_IMP_DIR ${EVIL_MATH_DIR}/imp) 

set(EVIL_TIMER_DIR ${EVIL_DIR}/timer)
set(EVIL_TIMER_IMP_DIR ${EVIL_TIMER_DIR}/imp) 

#the example dir - which is where we actually are - for completeness
set(EVIL_EXAMPLE_DIR ${EVIL_REPO_DIR}/examples) 
#the example src
set(EVIL_EXAMPLE_SRC_DIR ${EVIL_EXAMPLE_DIR}/src) 
set(EVIL_EXAMPLE_TEST_DIR ${EVIL_EXAMPLE_SRC_DIR}/tests) 

#setup compile options
if(${EVIL_IS_ROOT})
	#need to setup our global preprocessor, compile and link definitions 
	evil_add_preprocessor_def("UNICODE;_UNICODE")	
	if (${EVIL_IS_WINDOWS})
			evil_set_msvc_static_runtime()
			
			evil_add_preprocessor_def("_CRT_SECURE_NO_WARNINGS;NOMINMAX;STRICT")	
			evil_add_compile_options(" $<$<CONFIG:Release>:/Ox>")
			#evil_add_link_options()
	endif()

	if (${EVIL_IS_APPLE})
			#havent done osx yet
	endif()
		
	if (${EVIL_IS_UNIX})
			#haven't done unix yet
	endif()
endif()


#include paths - follow our own instructions and refer to the repo root
include_directories(${EVIL_REPO_DIR})

#grab the various source and headers

file(GLOB EVIL_ASSET_HEADERS ${EVIL_ASSET_DIR}/*.h)
file(GLOB EVIL_ASSET_SOURCE ${EVIL_ASSET_DIR}/*.cpp)
file(GLOB EVIL_ASSET_IMP_HEADERS ${EVIL_ASSET_IMP_DIR}/*.h)
file(GLOB EVIL_ASSET_IMP_SOURCE ${EVIL_ASSET_IMP_DIR}/*.cpp)
set(EVIL_ASSET_FILES ${EVIL_ASSET_HEADERS} ${EVIL_ASSET_SOURCE} 
					   ${EVIL_ASSET_IMP_SOURCE} ${EVIL_ASSET_IMP_HEADERS})

file(GLOB EVIL_SIGNALS_HEADERS ${EVIL_SIGNALS_DIR}/*.h)
file(GLOB EVIL_SIGNALS_SOURCE ${EVIL_SIGNALS_DIR}/*.cpp)
file(GLOB EVIL_SIGNALS_IMP_HEADERS ${EVIL_SIGNALS_IMP_DIR}/*.h)
file(GLOB EVIL_SIGNALS_IMP_SOURCE ${EVIL_SIGNALS_IMP_DIR}/*.cpp)
set(EVIL_SIGNALS_FILES ${EVIL_SIGNALS_HEADERS} ${EVIL_SIGNALS_SOURCE} 
					   ${EVIL_SIGNALS_IMP_SOURCE} ${EVIL_SIGNALS_IMP_HEADERS})
			
file(GLOB EVIL_TEST_RENDERER_HEADERS ${EVIL_TEST_RENDERER_DIR}/*.h)
file(GLOB EVIL_TEST_RENDERER_SOURCE ${EVIL_TEST_RENDERER_DIR}/*.cpp)
file(GLOB EVIL_TEST_RENDERER_IMP_HEADERS ${EVIL_TEST_RENDERER_IMP_DIR}/*.h)
file(GLOB EVIL_TEST_RENDERER_IMP_SOURCE ${EVIL_TEST_RENDERER_IMP_DIR}/*.cpp)
set(EVIL_TEST_RENDERER_FILES ${EVIL_TEST_RENDERER_HEADERS} ${EVIL_TEST_RENDERER_SOURCE} 
					   ${EVIL_TEST_RENDERER_IMP_SOURCE} ${EVIL_TEST_RENDERER_IMP_HEADERS})
					   
file(GLOB EVIL_TEST_HEADERS ${EVIL_TEST_DIR}/*.h)
file(GLOB EVIL_TEST_SOURCE ${EVIL_TEST_DIR}/*.cpp)
file(GLOB EVIL_TEST_IMP_SOURCE ${EVIL_TEST_IMP_DIR}/*.cpp)
set(EVIL_TEST_FILES ${EVIL_TEST_HEADERS} ${EVIL_TEST_SOURCE} 
					   ${EVIL_TEST_IMP_SOURCE})

file(GLOB EVIL_THREAD_HEADERS ${EVIL_THREAD_DIR}/*.h)
file(GLOB EVIL_THREAD_SOURCE ${EVIL_THREAD_DIR}/*.cpp) 
file(GLOB EVIL_THREAD_IMP_SOURCE ${EVIL_THREAD_IMP_DIR}/*.cpp) 
set(EVIL_THREAD_FILES ${EVIL_THREAD_HEADERS} ${EVIL_THREAD_SOURCE} ${EVIL_THREAD_IMP_SOURCE})

file(GLOB EVIL_UTIL_HEADERS ${EVIL_UTIL_DIR}/*.h)
file(GLOB EVIL_UTIL_SOURCE ${EVIL_UTIL_DIR}/*.cpp) 
file(GLOB EVIL_UTIL_IMP_SOURCE ${EVIL_UTIL_IMP_DIR}/*.cpp) 
set(EVIL_UTIL_FILES ${EVIL_UTIL_HEADERS} ${EVIL_UTIL_SOURCE} ${EVIL_UTIL_IMP_SOURCE})

file(GLOB EVIL_MATH_HEADERS ${EVIL_MATH_DIR}/*.h)
file(GLOB EVIL_MATH_SOURCE ${EVIL_MATH_DIR}/*.cpp) 
file(GLOB EVIL_MATH_IMP_SOURCE ${EVIL_MATH_IMP_DIR}/*.cpp) 
set(EVIL_MATH_FILES ${EVIL_MATH_HEADERS} ${EVIL_MATH_SOURCE} ${EVIL_MATH_IMP_SOURCE})

file(GLOB EVIL_TIMER_HEADERS ${EVIL_TIMER_DIR}/*.h)
file(GLOB EVIL_TIMER_SOURCE ${EVIL_TIMER_DIR}/*.cpp) 
file(GLOB EVIL_TIMER_IMP_SOURCE ${EVIL_TIMER_IMP_DIR}/*.cpp) 
set(EVIL_TIMER_FILES ${EVIL_TIMER_HEADERS} ${EVIL_TIMER_SOURCE} ${EVIL_TIMER_IMP_SOURCE})

file(GLOB EVIL_EXAMPLE_SOURCE ${EVIL_EXAMPLE_SRC_DIR}/*.cpp)
file(GLOB EVIL_EXAMPLE_TESTS ${EVIL_EXAMPLE_TEST_DIR}/*.h)


#create a the no build targets - used to organize files in visual studio
add_custom_target(${EVIL_NAME}-NOBUILD COMMENT "Project not built. Files are included by other targets" SOURCES ${EVIL_ASSET_FILES})

add_custom_target(evil-signals-NOBUILD COMMENT "Project not built. Files are included by other targets" SOURCES ${EVIL_SIGNALS_FILES})

add_custom_target(evil-test-renderer-NOBUILD COMMENT "Project not built. Files are included by other targets" SOURCES ${EVIL_TEST_RENDERER_FILES})

add_custom_target(test-harness-NOBUILD COMMENT "Project not built. Files are included by other targets" SOURCES ${EVIL_TEST_FILES})

add_custom_target(evil-thread-NOBUILD COMMENT "Project not built. Files are included by other targets" SOURCES ${EVIL_THREAD_FILES})

add_custom_target(evil-util-NOBUILD COMMENT "Project not built. Files are included by other targets" SOURCES ${EVIL_UTIL_FILES})

add_custom_target(evil-math-NOBUILD COMMENT "Project not built. Files are included by other targets" SOURCES ${EVIL_MATH_FILES})

add_custom_target(evil-timer-NOBUILD COMMENT "Project not built. Files are included by other targets" SOURCES ${EVIL_TIMER_FILES})

add_custom_target(test-cases-NOBUILD COMMENT "Project not built. Files are included by other targets" SOURCES ${EVIL_EXAMPLE_TESTS})

#build the samples  


#the samples and tests - that dont use GL
list(APPEND EXAMPLE_SOURCE src/run-tests.cpp)
#list(APPEND EXAMPLE_SOURCE src/sample-simple.cpp)
#list(APPEND EXAMPLE_SOURCE src/sample-threaded.cpp)

set(_EXR ${EVIL_STATIC_RELEASE_POSTFIX})
set(_EXD ${EVIL_STATIC_DEBUG_POSTFIX})
	
foreach(EXAMPLE ${EXAMPLE_SOURCE})
	get_filename_component(_NAME ${EXAMPLE} NAME_WE)

	set(_SOURCES ${EXAMPLE} )
	
	evil_add_executable(${_NAME} _SOURCES NONE)

	if (${EVIL_IS_WINDOWS})			
		set_target_properties(${_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
	endif()
					
endforeach()


#the samples and tests that require the GL subsystem

find_package(OpenGL REQUIRED)

list(APPEND GL_EXAMPLE_SOURCE src/glfw-tests.cpp)
#list(APPEND EVIL_TARGET_LINK_OPTIONS "${OPENGL_LIBRARIES}")
	  
#link_directories(${EVIL_EXAMPLE_DIR}deps/lib)

foreach(EXAMPLE ${GL_EXAMPLE_SOURCE})

	
	get_filename_component(_NAME ${EXAMPLE} NAME_WE)

	set(_SOURCES ${EXAMPLE} )
	
	evil_add_executable(${_NAME} _SOURCES NONE)

	target_include_directories(${_NAME} PUBLIC "deps/include")
	
	if ("${OPENGL_INCLUDE_DIR}")
		target_include_directories(${_NAME} PUBLIC "${OPENGL_INCLUDE_DIR}")
	endif()
  

	evil_target_link_libraries(${_NAME} STATIC ${EVIL_EXAMPLE_DIR}/deps/lib glfw glew zlib)
	 target_link_libraries(${_NAME} ${OPENGL_LIBRARIES})   
                 						
	if (${EVIL_IS_WINDOWS})			
		set_target_properties(${_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
	endif()
					
endforeach()

#evil_dump_all()

