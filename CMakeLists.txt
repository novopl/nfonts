project(nfonts)
cmake_minimum_required(VERSION 2.6)

option(BUILD_TESTS      "Build tests"       ON)

set(nfonts-src   src/main.cpp
                   )
set(nfonts-deps  )

include_directories(./include)

add_executable        (nfonts ${nfonts-src} )
target_link_libraries (nfonts ${nfonts-deps})


#========================================
# Qt4 support
find_package(Qt4 4.6.0 COMPONENTS QtCore QtGui QtNetwork REQUIRED )
if(QT4_FOUND)
  message("--------------------------------")
  message(STATUS "Using Qt4")
  message(STATUS "  include  : ${QT_INCLUDES}")
  message(STATUS "  libs     : ${QT_LIBRARIES}")
  message(STATUS "  defs     : ${QT_DEFINITIONS}")
  include(${QT_USE_FILE})
  include_directories ( ${QT_INCLUDES} )
  add_definitions     ( ${QT_DEFINITIONS} )
  list                (APPEND nfonts_deps   ${QT_LIBRARIES} )


  set(nfonts-qt-moc   include/nfonts/MainWindow.hpp
                    )
#  set(nfonts-qt-res src/qt/resources/resources.qrc)

  set(nfonts-qt-src    src/nfonts/MainWindow.cpp
                    )

  if(nfonts-qt-moc)
    QT4_WRAP_CPP(nfonts-moc-src ${nfonts-qt-moc})
  endif()

  if(nfonts-qt-res)
    qt4_add_resources(nfonts-res-src ${nfonts-qt-res})
  endif()

  list(APPEND nfonts_src ${nfonts-qt-src} ${nfonts-res-src} ${nfonts-moc-src})
endif()

#========================================
# Tests
if(BUILD_TESTS)
  find_package(CxxTest)
  if(CXXTEST_FOUND)
    include_directories(${CXXTEST_INCLUDE_DIR})
    message("--------------------------------")
    message(STATUS "Using CxxTest")
    message(STATUS "  include: ${CXXTEST_INCLUDE_DIR}")
    message(STATUS "  Tests:")

    aux_source_directory(tests  tests-src)
    set(tests_SOURCES           ${tests-src} )
    set(tests_DEPS              cppunit)

    enable_testing()
    foreach(t_src ${tests_SOURCES})
      get_filename_component(t_name ${t_src} NAME_WE)
      message(STATUS "    -${t_name}: ${t_src}")
      cxxtest_add_test(${t_name} ${t_name}.cxx ${CMAKE_CURRENT_SOURCE_DIR}/${t_src})
    endforeach()
  endif()
endif()
