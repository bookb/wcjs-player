cmake_minimum_required( VERSION 2.8 )

project( player_bindings )

if( MSVC )
    foreach( flag_var
             CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
             CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO )
        if( ${flag_var} MATCHES "/MD" )
            string( REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}" )
        endif()
    endforeach()
elseif( APPLE )
    set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/deps/VLC.app/Contents/MacOS/lib" )
else()
    add_definitions( -std=c++11 )
endif()

add_definitions( -DBUILDING_V8_SHARED )

file( GLOB SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/[^.]*.cpp
    src/[^.]*.h
    README.md
    package.json
    )

include_directories(
        ${CMAKE_JS_INC}
        deps
        node_modules/node-addon-api
        node_modules/node-addon-api-helpers
        $ENV{NODE_HOME}/include/node
        deps/libvlc_wrapper/include
)
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/deps/libvlc_wrapper )

add_library( ${PROJECT_NAME} SHARED ${SOURCE_FILES} )
set_target_properties( ${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node" )
target_link_libraries( ${PROJECT_NAME} ${CMAKE_JS_LIB}
        libvlc_wrapper )

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/build/Release)

#get_cmake_property( _variableNames VARIABLES )
#foreach( _variableName ${_variableNames} )
#    message( STATUS "${_variableName}=${${_variableName}}" )
#endforeach()
