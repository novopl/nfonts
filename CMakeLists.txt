project(nfonts)
cmake_minimum_required(VERSION 2.6)

option(BUILD_TESTS      "Build tests"       ON)

set(nfonts-src    src/nFontTypes.cpp
                  src/nGLGlyphAtlas.cpp
                  src/nFontFace.cpp
                  src/nFont.cpp
                  src/nFontRenderers.cpp
                  src/nSDLFramework.cpp)
set(nfonts-deps   )
set(nfonts-inc    )

include_directories(../common/include)
include_directories(./include)
include_directories(./lib/nlogger/include)

#========================================
# OpenGL
find_package        ( OpenGL REQUIRED )
# include_directories ( ${OPENGL_INCLUDE_DIR} )
list                ( APPEND nfonts-inc ${OPENGL_INCLUDE_DIR} )
list                ( APPEND nfonts-deps  ${OPENGL_gl_LIBRARY}
                                          ${OPENGL_glu_LIBRARY} )
# Freetype
find_package        ( Freetype REQUIRED )
# include_directories ( ${FREETYPE_INCLUDE_DIRS} )
list                ( APPEND nfonts-inc ${FREETYPE_INCLUDE_DIRS} )
list                ( APPEND nfonts-deps ${FREETYPE_LIBRARIES} )
# SDL
find_package        ( SDL REQUIRED )
# include_directories ( ${SDL_INCLUDE_DIR} )
list                ( APPEND nfonts-inc ${SDL_INCLUDE_DIR} )
list                ( APPEND nfonts-deps    ${SDL_LIBRARY} )

foreach( inc_dir ${nfonts-inc} )
  include_directories( ${inc_dir} )
endforeach()

# Enable C++11 support
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-std=c++0x")        ## Optimize
endif()

add_library           (nfonts ${nfonts-src} )
target_link_libraries (nfonts ${nfonts-deps})

add_executable        (main_nfonts  src/main.cpp )
target_link_libraries (main_nfonts  nfonts)

#========================================
# Tests
if(BUILD_TESTS)
  find_package(CxxTest)
  if(CXXTEST_FOUND)
    aux_source_directory(tests  tests-src)
    if( tests-src )
      include_directories(${CXXTEST_INCLUDE_DIR})
      set(tests_SOURCES           ${tests-src} )
      set(tests_DEPS              cppunit nfonts)

      enable_testing()
      foreach(t_src ${tests_SOURCES})
        get_filename_component(t_name ${t_src} NAME_WE)
        message(STATUS "    -${t_name}: ${t_src}")
        cxxtest_add_test(${t_name} ${t_name}.cxx ${CMAKE_CURRENT_SOURCE_DIR}/${t_src})
        target_link_libraries (${t_name} ${tests_DEPS})
      endforeach()
    endif()
  endif()
endif()
