cmake_minimum_required(VERSION 2.8)
project(rr)

# These will be the defaults ...
if(WIN32)
    SET(RR_BUILDS_ROOT      "c:/builds")
    SET(RR_INSTALLS_ROOT    "c:/installs")

    if(BORLAND)
        set(FOLDER_POSTFIX "xe")
    endif()

    if(MSVC)
        set(FOLDER_POSTFIX "vs")
    endif()

    if(UNIX)
        set(FOLDER_POSTFIX "gcc")
    endif()

    set(THIRD_PARTY_INSTALL_FOLDER ${RR_INSTALLS_ROOT}/${FOLDER_POSTFIX}/release CACHE PATH "Directory containing ThirdParty libraries.")
    if("${THIRD_PARTY_INSTALL_FOLDER}" STREQUAL "")
        set(THIRD_PARTY_INSTALL_FOLDER ${RR_INSTALLS_ROOT}/${FOLDER_POSTFIX}/release CACHE PATH "Directory containing ThirdParty libraries" FORCE)
    endif()
else()
    # Default the third party dir to a more conventional unix type path
    # This would typically be /usr/local, $HOME/local or something like that.
    set(THIRD_PARTY_INSTALL_FOLDER ${CMAKE_INSTALL_PREFIX} CACHE PATH "Directory containing ThirdParty libraries.")
endif(WIN32)

# add cmake files to find things, prepend with our path
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/ ${CMAKE_MODULE_PATH})


set(LLVM_INSTALL_PREFIX CACHE PATH "If LLVM was built using CMake, this is the location where it was installed")

# determine if LLVM was installed using CMake
if(EXISTS ${LLVM_INSTALL_PREFIX}/share/llvm/cmake)
    message(STATUS "Found CMake built LLVM")

    option (BUILD_LLVM "Build the LLVM back end" ON)
    set(BUILD_LLVM ON)
    set(LLVM_FOUND TRUE)

    # Use the propper LLVM supplied CMake files.
    # prepend the cmake module path with the LLVM modules
    set(CMAKE_MODULE_PATH "${LLVM_INSTALL_PREFIX}/share/llvm/cmake" ${CMAKE_MODULE_PATH})

    include(LLVMConfig)

    # we're building a JIT compiler with support for binary code (no interpreter):
    # this sets the LLVM_LIBRARIES var to be the list of required LLVM libs
    # to link with.
    llvm_map_components_to_libraries(LLVM_LIBRARIES jit native)

else()
    message(STATUS "Looking for LLVM installed without CMake")

    set(LLVM_MIN_VERSION       "3000000")
    set(LLVM_MIN_VERSION_TEXT  "3.0")

    # look for LLVM using the llvm-config program
    find_package(LLVM)
endif()

# should we use LLVM ?
option (BUILD_LLVM "Build the LLVM back end" OFF)

# should we build the swig python wrapper?
option (BUILD_SWIG_PYTHON "build the *EXPERIMENTAL* SWIG generated python wrapper" OFF)

if(BUILD_LLVM AND NOT LLVM_FOUND)
    message(FATAL_ERROR "BUILD_LLVM is enabled, but no LLVM installation was found")
endif()

mark_as_advanced(
    INSTALL_APPS
    INSTALL_CXX_API
    INSTALL_C_API
    INSTALL_PYTHON
    INSTALL_STATIC_LIB
    )

set(RR_GENERATED_HEADER_PATH ${CMAKE_CURRENT_BINARY_DIR}/source)
set(RR_ROADRUNNER_INSTALL_PATH ${CMAKE_INSTALL_PREFIX})
set(RR_ROADRUNNER_INSTALL_LIB_PATH ${RR_ROADRUNNER_INSTALL_PATH}/lib)
set(RR_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

OPTION (INSTALL_CXX_API          "Generate C++ api"                        ON )
OPTION (INSTALL_C_API            "Generate C API"                          ON )
OPTION (INSTALL_PYTHON           "Package Python wrapper"                  ON )
OPTION (INSTALL_APPS             "Build and install Apps"                  ON )
OPTION (INSTALL_EXAMPLES         "Build and install Examples"              OFF)
OPTION (INSTALL_STATIC_LIB       "Install RoadRunner CXX static lib"       OFF)
OPTION (RR_BUILD_SHARED_CORE     "Build RoadRunner Core Shared library"    OFF)
OPTION (BUILD_TESTS              "Build the SBML C API test suite"         OFF)
OPTION (INSTALL_SBML_MODELS      "Install SBML Models"                     ON )
OPTION (BUILD_PLUGINS            "Build and Install Plugins"               ON )

#Setup so that roadrunner is always built as a dll and linked statically with 'as much as possible'
set(BUILD_SHARED_LIBS            ON)

set(RR_ROOT                      ${CMAKE_CURRENT_SOURCE_DIR})
set(RR_SOURCE_ROOT               ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(THIRD_PARTY_FOLDER           ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

set(exe_path                     ${PROJECT_BINARY_DIR}/bin)
set(lib_path                     ${PROJECT_BINARY_DIR}/lib)

message(STATUS "Using third party library prefix of ${THIRD_PARTY_INSTALL_FOLDER}")
message(STATUS "Installing RoadRunner to ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
    "Choose the build type. The options are: None (CMAKE_CXX_FLAGS or
     CMAKE_C_FLAGS are used), Debug, Release, RelWithDebInfo, MinSizeRel.")
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
        "Choose the build type. The options are: None (CMAKE_CXX_FLAGS or
  CMAKE_C_FLAGS are used), Debug, Release, RelWithDebInfo, MinSizeRel." FORCE)
endif()

IF(WIN32)
    IF(${MSVC})
        IF(CMAKE_BUILD_TYPE MATCHES "Debug")
            set(exe_path ${exe_path}/Debug)
            set(lib_path ${lib_path}/Debug)
        ELSE(CMAKE_BUILD_TYPE MATCHES "Debug")
               set(exe_path ${exe_path}/Release)
               set(lib_path ${lib_path}/Release)
        ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")
    ELSE(WIN32)
   # Linux
    ENDIF(${MSVC})
ENDIF(WIN32)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY     ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY     ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY     ${PROJECT_BINARY_DIR}/bin)

include_directories(
    ${RR_ROOT}
    ${THIRD_PARTY_FOLDER}
)

# TODO libxml2 handling is excedingly hackish, it is all homegrown
# manually linking crap, need to fix this!

if (WIN32)
    message(STATUS "adding ${THIRD_PARTY_INSTALL_FOLDER}/include/libxml2 to include path")
    include_directories(${THIRD_PARTY_INSTALL_FOLDER}/include/libxml2)
else()
    #UNIX
    find_package(LibXml2 REQUIRED)
	message(STATUS "found libxml2, include dir: ${LIBXML2_INCLUDE_DIR}") 
	include_directories(${LIBXML2_INCLUDE_DIR})
endif(WIN32)


if (${BUILD_LLVM})
    link_directories(
        ${LIBRARY_OUTPUT_PATH}
        ${THIRD_PARTY_FOLDER}/dependencies/libsbml/lib
        ${THIRD_PARTY_INSTALL_FOLDER}/lib
        ${LLVM_LIBRARY_DIRS}
    )
else()
    link_directories(
        ${LIBRARY_OUTPUT_PATH}
        ${THIRD_PARTY_FOLDER}/dependencies/libsbml/lib
        ${THIRD_PARTY_INSTALL_FOLDER}/lib
    )
endif (${BUILD_LLVM})

if(${MSVC})
    add_definitions(
        -DCRT_NONSTDC_NO_DEPRECATE
        -DPOCO_NO_AUTOMATIC_LIBS
        -D_CRT_SECURE_NO_WARNINGS   #MS wanting you to changes basically all standard C functions :(
        )
    SET (CMAKE_CXX_FLAGS             "${CMAKE_CXX_FLAGS} /EHsc ")
    SET (CMAKE_SHARED_LINKER_FLAGS     "${CMAKE_SHARED_LINKER_FLAGS} ") #/FORCE:MULTIPLE")
endif()

#=== COMPILER FLAGS
#Common compiler definitions
add_definitions(
    -DLIBSBML_USE_CPP_NAMESPACE
)

if(${MINGW})
    SET (CMAKE_CXX_FLAGS             "${CMAKE_CXX_FLAGS} -std=gnu++0x")
endif()

if(${BORLAND})
    link_directories(${THIRD_PARTY_FOLDER}/dependencies/libsbml/lib)
add_definitions(
    -DUSE_PCH                   #Enable pre-compiled headers
    -H=${PROJECT_BINARY_DIR}/rr_pch.csm
    -w-8012                     #Comparing signed /unsigned
    -w-8057                     #Parameter never used
    -w-8004                     #'var' is assigned a value that is never used
    )
endif()

if(UNIX)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    endif()
endif()

if(BUILD_LLVM)
    message(STATUS "using LLVM, version ${LLVM_VERSION_MAJOR}.${LLVM_VERSION_MINOR}.${LLVM_VERSION_PATCH}")

    message(STATUS "LLVM_LIBRARIES: ${LLVM_LIBRARIES}")
    message(STATUS "LLVM_DEFINITIONS: ${LLVM_DEFINITIONS}")
    message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
    message(STATUS "LLVM_LIBRARY_DIRS: ${LLVM_LIBRARY_DIRS}")
    add_definitions(-DBUILD_LLVM)
    add_definitions(${LLVM_DEFINITIONS})
    include_directories(${LLVM_INCLUDE_DIRS})
else()
    message(STATUS "NOT using LLVM")
endif(BUILD_LLVM)


# We alway have to build the roadrunner core, everything depends on this
add_subdirectory(source)

if(INSTALL_CXX_API)
    if(INSTALL_APPS)
        add_subdirectory(apps)
    endif()

    if(INSTALL_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif(INSTALL_CXX_API)

add_subdirectory(wrappers)

if(BUILD_TESTS)
    message(STATUS "building tests")
    add_subdirectory(testing)
else()
    message(STATUS "NOT building tests")
endif(BUILD_TESTS)

if(BUILD_PLUGINS)
    message(STATUS "building plugins")
    add_subdirectory(plugins)
else()
    message(STATUS "NOT building plugins")
endif(BUILD_PLUGINS)

#======================= INSTALL STUFF ========================================================
#What sub folders to move into...?
if(${INSTALL_SBML_MODELS})
    add_subdirectory(models)
endif()

if(INSTALL_CXX_API)
    install(FILES source/rr_pch.h
        DESTINATION include
        COMPONENT rr_core)
endif(INSTALL_CXX_API)

install(FILES Readme.txt License.txt
        DESTINATION .
        COMPONENT info
        )

#=== ThirdParties
if(${BORLAND})
    set(CG_RUNTIMES        cc32110MT.dll)
    foreach(runtime ${CG_RUNTIMES})
        install (FILES
            ${THIRD_PARTY_FOLDER}/dependencies/cg/xe/${runtime}
            DESTINATION bin
            COMPONENT rr_core)
    endforeach()
endif()

if(WIN32)
#One day these may be statically linked
set(RUNTIMES libxml2.DLL iconv.dll zlib1.dll)
foreach(runtime ${RUNTIMES})
    install(    FILES     ${THIRD_PARTY_FOLDER}/dependencies/libsbml/bin/${runtime}
        DESTINATION bin
        COMPONENT rr_core)
endforeach()
endif()
