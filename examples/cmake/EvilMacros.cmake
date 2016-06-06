#the source tree contains various subprojects that can be broken out and built as 
#independent projects and we end up in a hell of macro definition incompatibility
#with seperate macro files in every independent subproject. To solve it copy the 
#top level macro file into every subproject
macro (evil_update_macro_files _macrodir _macrofile)
	set(_original ${_macrodir}/${_macrofile})

	file(GLOB_RECURSE XList  ${_macrofile})

	foreach (X ${XList})
		if(NOT (${X} STREQUAL ${_original} ))	
			GET_FILENAME_COMPONENT(_dir ${X} PATH)
		  file(COPY ${_original} DESTINATION ${_dir})
		endif()
	endforeach()
endmacro()

#dump some var
macro (evil_dump_var _name _variable)
	message(STATUS ">>> ${_name}=${_variable}")	
endmacro()

#dump everything
macro (evil_dump_all)
	get_cmake_property(_variableNames VARIABLES)
	foreach (_variableName ${_variableNames})
		message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()
endmacro()

macro (evil_build_static)
	set(BUILD_SHARED_LIBS FALSE)
endmacro()

macro (evil_setup_environment)
	if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
		set(EVIL_IS_ROOT TRUE)
		evil_setup_root_environment()
	else()
		set(EVIL_IS_ROOT FALSE)
		evil_setup_subenvironment()
	endif()
endmacro()

macro (evil_setup_root_environment)
	evil_identify_platform()
	evil_setup_target_strings()
	
	#empty global compile options
	set(EVIL_PREPROCESSOR_DEF) 
	set(EVIL_COMPILE_OPTIONS )
	set(EVIL_LINK_OPTIONS )
	
	get_filename_component(EVIL_ROOT "${CMAKE_SOURCE_DIR}/" ABSOLUTE)
	get_filename_component(EVIL_CURRENT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/" ABSOLUTE)
		
	set(EVIL_BIN ${EVIL_ROOT}/bin)
	set(EVIL_LIB ${EVIL_ROOT}/lib)
	set(EVIL_INCLUDE ${EVIL_ROOT}/include)

endmacro()


macro (evil_setup_subenvironment)
	get_filename_component(EVIL_CURRENT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/" ABSOLUTE)
	set(EVIL_TARGET_PREPROCESSOR_DEF )
	set(EVIL_TARGET_COMPILE_OPTIONS )
	set(EVIL_TARGET_LINK_OPTIONS )
endmacro()

macro (evil_identify_platform)
	set(EVIL_IS_UNKNOWN TRUE)
	set(EVIL_IS_WINDOWS FALSE)
	set(EVIL_IS_OSX FALSE)
	set(EVIL_IS_UNIX FALSE)
	
	if(MSVC)
		set(EVIL_IS_UNKNOWN FALSE)
		set(EVIL_IS_WINDOWS TRUE)
	elseif(APPLE)
		set(EVIL_IS_UNKNOWN FALSE)
		set(EVIL_IS_OSX TRUE)
	elseif(UNIX)
		set(EVIL_IS_UNKNOWN FALSE)
		set(EVIL_IS_UNIX TRUE)
	else()
		message( FATAL_ERROR "Unknown environment CMake will exit" )
	endif()
endmacro()


macro (evil_add_preprocessor_def _DEF)
	if(${EVIL_IS_ROOT})
		set(EVIL_PREPROCESSOR_DEF ${EVIL_PREPROCESSOR_DEF} ${_DEF})
	else()
		set(EVIL_TARGET_PREPROCESSOR_DEF ${EVIL_TARGET_PREPROCESSOR_DEF} ${_DEF})
	endif()
endmacro()

macro (evil_add_compile_options _OPT)
	if(${EVIL_IS_ROOT})
		set(EVIL_COMPILE_OPTIONS ${EVIL_OMPILE_OPTIONS} ${_OPT})
	else()
		set(EVIL_TARGET_COMPILE_OPTIONS ${EVIL_TARGET_COMPILE_OPTIONS} ${_OPT})
	endif()
endmacro()

macro (evil_add_link_options _OPT)
	if(${EVIL_IS_ROOT})
		set(EVIL_LINK_OPTIONS ${EVIL_LINK_OPTIONS} ${_OPT})
	else()
		set(EVIL_TARGET_LINK_OPTIONS ${EVIL_TARGET_LINK_OPTIONS} ${_OPT})
	endif()
endmacro()


macro (evil_consolidate_build_options _TARGET)

	set(EVIL_PREPROCESSOR_DEF ${EVIL_TARGET_PREPROCESSOR_DEF} ${EVIL_PREPROCESSOR_DEF})
	set(EVIL_COMPILE_OPTIONS ${EVIL_TARGET_COMPILE_OPTIONS} ${EVIL_COMPILE_OPTIONS})
	set(EVIL_LINK_OPTIONS ${EVIL_TARGET_LINK_OPTIONS} ${EVIL_LINK_OPTIONS})
	
	target_compile_definitions(${_TARGET} PRIVATE ${EVIL_PREPROCESSOR_DEF})
	target_compile_options(${_TARGET} PRIVATE ${EVIL_COMPILE_OPTIONS})

	#annoying but cmake not handling if(list) very well
	set(X )
	list(LENGTH EVIL_LINK_OPTIONS X)
	if(X GREATER 0)
		set_target_properties(${_TARGET} PROPERTIES LINK_FLAGS  ${EVIL_LINK_OPTIONS} )
	endif()

	
	#SET( CMAKE_EXE_LINKER_FLAGS  ${CMAKE_EXE_LINKER_FLAGS} ${EVIL_LINK_OPTIONS} )
	#SET( CMAKE_EXE_LINKER_FLAGS  ${CMAKE_EXE_LINKER_FLAGS} ${EVIL_LINK_OPTIONS} )
	#target_link_libraries(${_TARGET} PRIVATE ${EVIL_LINK_OPTIONS})
	#SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${EVIL_COMPILE_OPTIONS}" )

endmacro()

macro (evil_copy_binaries _TARGET)

	add_custom_command(TARGET ${_TARGET} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/lib/" "${EVIL_ROOT}/lib")		
														  
	 add_custom_command(TARGET ${_TARGET} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/bin/" "${EVIL_ROOT}/bin")		
	
endmacro()

macro (evil_copy_files _TARGET _FILES _DESTINATION)

	foreach(_FILE ${_FILES})		

		add_custom_command(TARGET ${_TARGET} POST_BUILD 
				 COMMAND ${CMAKE_COMMAND} -E copy ${_FILE}  ${_DESTINATION})		
	endforeach()
	
endmacro()

macro (evil_copy_files_relative _TARGET _ROOT _FILES _DESTINATION)

	foreach(_FILE ${_FILES})		

		add_custom_command(TARGET ${_TARGET} POST_BUILD 
				 COMMAND ${CMAKE_COMMAND} -E copy ${_ROOT}/${_FILE}  ${_DESTINATION})		
	endforeach()
	
endmacro()

												  
macro (evil_copy_single_file _TARGET _FROM _TO)
	add_custom_command(TARGET ${_TARGET} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy ${_FROM} ${_TO} )	
endmacro()

macro (evil_copy_directory _TARGET _FROM _TO)
	add_custom_command(TARGET ${_TARGET} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${_FROM} ${_TO} )
endmacro()

macro (evil_target_link_libraries _TARGET_NAME _TYPE _PATH)
#_TYPE one of SHARED or STATIC 

	set(_LIBS ${ARGN})
	set(_EXT )
	set(_EXTD )
	set(_DEBUG_LIBS )
	set(_RELEASE_LIBS )
	
	if(NOT EVIL_PLATFORM)
		evil_setup_target_strings()
	endif()
	
	if(${_TYPE} STREQUAL "SHARED")
	
		set(_EXT ${EVIL_SHARED_RELEASE_POSTFIX})
		set(_EXTD ${EVIL_SHARED_DEBUG_POSTFIX})
		
		foreach(_LIB ${_LIBS})
		
			#release
			set(X ${_LIB}${_EXT}_LOCATION)
			find_library(${_LIB}${_EXT}_LOCATION ${_LIB}${_EXT} PATH ${_PATH})
			target_link_libraries(${_TARGET_NAME} optimized ${${X}})
		
			#debug
			set(X ${_LIB}${_EXTD}_LOCATION)
			find_library(${_LIB}${_EXTD}_LOCATION ${_LIB}${_EXTD} PATH ${_PATH})
			target_link_libraries(${_TARGET_NAME} debug ${${X}})
			
		endforeach()
		
	elseif(${_TYPE} STREQUAL "STATIC")
	
		set(_EXT ${EVIL_STATIC_RELEASE_POSTFIX})
		set(_EXTD ${EVIL_STATIC_DEBUG_POSTFIX})
		
		foreach(_LIB ${_LIBS})
			#release
			set(X ${_LIB}${_EXT}_LOCATION)
			find_library(${_LIB}${_EXT}_LOCATION ${_LIB}${_EXT} PATH ${_PATH})
			target_link_libraries(${_TARGET_NAME} optimized ${${X}})
		
			#debug
			set(X ${_LIB}${_EXTD}_LOCATION)
			find_library(${_LIB}${_EXTD}_LOCATION ${_LIB}${_EXTD} PATH ${_PATH})
			target_link_libraries(${_TARGET_NAME} debug ${${X}})		
		endforeach()
	
	elseif(_TYPE STREQUAL OTHER)
		
		set(_BUILD general)
		foreach(_LIB ${_LIBS})
		
			if (${_LIB} STREQUAL optimized OR ${_LIB} STREQUAL debug OR ${_LIB} STREQUAL general)
                set (_BUILD ${_LIB})
            else()
			   find_library(${_LIB}${_BUILD}_LOCATION ${_LIB} PATH ${_PATH})
               target_link_libraries(${_TARGET_NAME} ${_BUILD} ${_LIB}${_BUILD}_LOCATION)             
            endif()
			
		endforeach()
	
	endif()
	
endmacro()


macro(evil_set_msvc_static_runtime)

	foreach(_BUILD_CONFIG DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
		foreach(_BUILD_VARS CMAKE_CXX_FLAGS_${_BUILD_CONFIG} CMAKE_C_FLAGS_${_BUILD_CONFIG})
			string(REGEX REPLACE /MD /MT ${_BUILD_VARS} ${${_BUILD_VARS}})
		endforeach()
	endforeach()
	
endmacro()
   

macro(evil_setup_target_strings)
	
	set(EVIL_PLATFORM )
	
	set(X )
	if(NOT WIN32)
		set(EVIL_PLATFORM "osx" )
	else()
		if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
			set(EVIL_PLATFORM x64)	
			set(X 64)			
		else()
			set(EVIL_PLATFORM Win32)
			set(X 32)
		endif()
	endif()


	set(EVIL_SHARED_RELEASE_POSTFIX ${X} )
	set(EVIL_SHARED_DEBUG_POSTFIX ${X}d )
	set(EVIL_STATIC_RELEASE_POSTFIX ${X}s )
	set(EVIL_STATIC_DEBUG_POSTFIX ${X}sd )
	set(EVIL_EXECUTABLE_RELEASE_POSTFIX ${X} )
	set(EVIL_EXECUTABLE_DEBUG_POSTFIX ${X}d )
	
endmacro()

macro(evil_set_postfix _TYPE)

	if(NOT EVIL_PLATFORM)
		evil_setup_target_strings()
	endif()
		
	if(${_TYPE}  STREQUAL "SHARED")
		set(CMAKE_RELEASE_POSTFIX ${EVIL_SHARED_RELEASE_POSTFIX})
		set(CMAKE_DEBUG_POSTFIX	${EVIL_SHARED_DEBUG_POSTFIX})
	elseif(${_TYPE}  STREQUAL "STATIC")
		set(CMAKE_RELEASE_POSTFIX ${EVIL_STATIC_RELEASE_POSTFIX})
		set(CMAKE_DEBUG_POSTFIX ${EVIL_STATIC_DEBUG_POSTFIX})
	elseif(${_TYPE}  STREQUAL "EXECUTABLE")
		set(CMAKE_RELEASE_POSTFIX ${EVIL_EXECUTABLE_RELEASE_POSTFIX})
		set(CMAKE_DEBUG_POSTFIX	${EVIL_EXECUTABLE_DEBUG_POSTFIX})	
	endif()
	
endmacro()

macro(evil_set_target_props _NAME)

	if(${EVIL_IS_WINDOWS})
	
		 set_target_properties(${_NAME} PROPERTIES
		 
			ARCHIVE_OUTPUT_NAME ${_NAME}   
			ARCHIVE_OUTPUT_DIRECTORY ${EVIL_LIB}
			ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${EVIL_LIB}
			ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${EVIL_LIB}
			#ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${EVIL_LIB}
			#ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVIL_LIB}
			
			COMPILE_PDB_NAME ${_NAME}${CMAKE_DEBUG_POSTFIX}
			COMPILE_PDB_OUTPUT_DIRECTORY_DEBUG ${EVIL_LIB}
			COMPILE_PDB_OUTPUT_DIRECTORY_RELEASE ${EVIL_LIB}
			#COMPILE_PDB_OUTPUT_DIRECTORY_MINSIZEREL ${EVIL_LIB}
			#COMPILE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVIL_LIB}
			
			LIBRARY_OUTPUT_NAME ${_NAME}   
			LIBRARY_OUTPUT_DIRECTORY ${EVIL_BIN}
			LIBRARY_OUTPUT_DIRECTORY_DEBUG ${EVIL_BIN}
			LIBRARY_OUTPUT_DIRECTORY_RELEASE ${EVIL_BIN}
			#LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${EVIL_BIN}n
			#LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVIL_BIN}
			
			RUNTIME_OUTPUT_NAME ${_NAME}   
			RUNTIME_OUTPUT_NAME_DEBUG ${_NAME}${EVIL_EXECUTABLE_DEBUG_POSTFIX} 
			RUNTIME_OUTPUT_NAME_RELEASE ${_NAME}${EVIL_EXECUTABLE_RELEASE_POSTFIX} 
			RUNTIME_OUTPUT_DIRECTORY ${EVIL_BIN}
			RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EVIL_BIN}
			RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EVIL_BIN}
			#RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${EVIL_BIN}
			#RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVIL_BIN}		
		)
		
		set(CMAKE_PDB_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_DIRECTORY})
		
	else()
	
		set_target_properties(${_NAME} PROPERTIES
		
			ARCHIVE_OUTPUT_NAME ${_NAME}   
			ARCHIVE_OUTPUT_DIRECTORY ${EVIL_LIB}
			ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${EVIL_LIB}
			ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${EVIL_LIB}
			#ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${EVIL_LIB}
			#ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVIL_LIB}
					
			LIBRARY_OUTPUT_NAME ${_NAME}   
			LIBRARY_OUTPUT_DIRECTORY ${EVIL_LIB}
			LIBRARY_OUTPUT_DIRECTORY_DEBUG ${EVIL_LIB}
			LIBRARY_OUTPUT_DIRECTORY_RELEASE ${EVIL_LIB}
			#LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${EVIL_LIB}
			#LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVIL_LIB}
			
			RUNTIME_OUTPUT_NAME ${_NAME}   
			RUNTIME_OUTPUT_NAME_RELEASE ${_NAME}${EVIL_EXECUTABLE_RELEASE_POSTFIX} 
			RUNTIME_OUTPUT_NAME_DEBUG ${_NAME}${EVIL_EXECUTABLE_DEBUG_POSTFIX}  
			RUNTIME_OUTPUT_DIRECTORY ${EVIL_BIN}
			RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EVIL_BIN}
			RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EVIL_BIN}
			#RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${EVIL_BIN}
			#RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${EVIL_BIN}	
				
		)
		
	endif()
		
endmacro()

macro(evil_replace_cmakefiles _NAME _DEST)
	file(COPY ${EVIL_CURRENT_ROOT}/cmake/replacements/${_NAME}/CMakeLists.txt DESTINATION ${EVIL_CURRENT_ROOT}/${_DEST})
endmacro()


macro(evil_add_executable _NAME _SOURCES_VAR _HEADERS_VAR)
	evil_set_postfix("EXECUTABLE")
    add_executable(${_NAME} ${${_SOURCES_VAR}} ${${_HEADERS_VAR}})
	evil_set_target_props(${_NAME})
	evil_consolidate_build_options(${_NAME})	
	
endmacro()

macro(evil_add_static_lib _NAME _SOURCES_VAR _HEADERS_VAR)
	evil_set_postfix("STATIC")
    add_library(${_NAME} STATIC ${${_SOURCES_VAR}} ${${_HEADERS_VAR}})
	evil_set_target_props(${_NAME})
	evil_consolidate_build_options(${_NAME})	
endmacro()

macro(evil_add_shared_lib _NAME _SOURCES_VAR _HEADERS_VAR)
	evil_set_postfix("SHARED")
    add_library(${_NAME} SHARED ${${_SOURCES_VAR}} ${${_HEADERS_VAR}})
	evil_set_target_props(${_NAME})
	evil_consolidate_build_options(${_NAME})
endmacro()
