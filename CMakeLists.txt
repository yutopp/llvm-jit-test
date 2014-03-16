PROJECT( llvm-jit )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

set( PACKAGE_VERSION "${RILL_VERSION_MAJOR}.${RILL_VERSION_MINOR}" )


#
include_directories( "${PROJECT_SOURCE_DIR}" )

#
add_definitions(
    -std=c++11
#    -Wall
)


#
# LLVM
#
# ref. http://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
find_package( LLVM )

if( NOT LLVM_FOUND )
  message( FATAL_ERROR "LLVM package can't be found. Set CMAKE_PREFIX_PATH variable to LLVM's installation prefix." )
else()
  message( STATUS "LLVM lib: ${LLVM_LIBRARY_DIRS}" )
endif()

include_directories( ${LLVM_INCLUDE_DIRS} )
link_directories( ${LLVM_LIBRARY_DIRS} )
add_definitions( ${LLVM_DEFINITIONS} )

llvm_map_components_to_libraries(
  REQ_LLVM_LIBRARIES
  native
  support
  bitreader
  instrumentation
  scalaropts
  ipo
  vectorize
  core
  object
  jit
  interpreter
  nativecodegen
  selectiondag
  asmparser
  asmprinter
  )

# the LLMV's "target_link_libraries" is specified at lib/rill/CMakeLists.txt


file( GLOB_RECURSE files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )

# create executable per file
foreach( file ${files} )
  message( STATUS ${file} )
  get_filename_component( basename ${file} NAME_WE )
  message( STATUS ${basename} )

  add_executable( ${basename} ${file} )
  target_link_libraries(
    ${basename}
    ${REQ_LLVM_LIBRARIES}
    )
  set_target_properties( ${basename} PROPERTIES LINKER_LANGUAGE CXX )
endforeach()


