project( calamares CXX )

# The partition manager uses ECM but ECMConfig.cmake
# will complain if we require CMake less than 2.8.13,
# so never change this.
cmake_minimum_required( VERSION 2.8.12 )

set( CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules" )

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    message( STATUS "Found Clang ${CMAKE_CXX_COMPILER_VERSION}, setting up Clang-specific compiler flags." )
    set( CMAKE_C_FLAGS                  "${CMAKE_C_FLAGS} -Wall -std=c99" )
    set( CMAKE_C_FLAGS_DEBUG            "-g" )
    set( CMAKE_C_FLAGS_MINSIZEREL       "-Os -DNDEBUG" )
    set( CMAKE_C_FLAGS_RELEASE          "-O4 -DNDEBUG" )
    set( CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g" )

    set( CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -Weverything -std=c++11" )
    set( CMAKE_CXX_FLAGS_DEBUG          "-g" )
    set( CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG" )
    set( CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG" )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g" )

    set( CMAKE_TOOLCHAIN_PREFIX "llvm-" )

    set( CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined" )
else()
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--no-undefined" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--fatal-warnings -Wnon-virtual-dtor -Woverloaded-virtual -Werror=return-type" )
endif()

if( CMAKE_COMPILER_IS_GNUCXX )
    if( CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9 OR
        CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.9 )
        message( STATUS "Found GNU g++ ${CMAKE_CXX_COMPILER_VERSION}, enabling colorized error messages." )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=auto" )
    endif()
endif()

cmake_policy( SET CMP0023 OLD )

# Keep cmake 3.0 quiet
if( POLICY CMP0043 )
    cmake_policy( SET CMP0043 OLD )
endif()

include( MacroOptionalFindPackage )
include( MacroLogFeature )

set( QT_VERSION 5.3.0 )

find_package( Qt5 ${QT_VERSION} CONFIG REQUIRED Core Gui Widgets LinguistTools Svg Quick QuickWidgets )
find_package( YamlCpp 0.5.1 REQUIRED )
find_package( PolkitQt5-1 REQUIRED )

option( WITH_PYTHON "Enable Python modules support." ON )

macro_optional_find_package( PythonLibs 3.3 )
macro_log_feature(
    PYTHONLIBS_FOUND
    "Python"
    "C interface libraries for the Python 3 interpreter."
    "http://python.org"
    FALSE "3.3"
    "Python 3 is used for some Calamares job modules."
)

if ( PYTHONLIBS_FOUND )
    include( BoostPython3 )
    find_boost_python3( 1.54.0 ${PYTHONLIBS_VERSION_STRING} CALAMARES_BOOST_PYTHON3_FOUND )
    macro_log_feature(
        CALAMARES_BOOST_PYTHON3_FOUND
        "Boost.Python"
        "A C++ library which enables seamless interoperability between C++ and Python 3."
        "http://www.boost.org"
        FALSE "1.54.0"
        "Boost.Python is used for interfacing with Calamares job modules written in Python 3."
    )
endif()

if ( PYTHONLIBS_NOTFOUND OR NOT CALAMARES_BOOST_PYTHON3_FOUND )
    set( WITH_PYTHON OFF )
endif()

###
### Calamares application info
###
set( CALAMARES_ORGANIZATION_NAME "Calamares" )
set( CALAMARES_ORGANIZATION_DOMAIN "github.com/calamares" )
set( CALAMARES_APPLICATION_NAME  "Calamares" )
set( CALAMARES_DESCRIPTION_SUMMARY "The distribution-independent installer framework" )
set( CALAMARES_TRANSLATION_LANGUAGES ar ast ca cs_CZ da de en en_GB es_MX es fi_FI fr hr hu id it_IT ja lt nl pl pt_BR pt_PT ro ru sk sl sv tr_TR zh_CN zh_TW )

set( CALAMARES_VERSION_MAJOR 1 )
set( CALAMARES_VERSION_MINOR 1 )
set( CALAMARES_VERSION_PATCH 0 )
set( CALAMARES_VERSION_RC 1 )

set( CALAMARES_VERSION ${CALAMARES_VERSION_MAJOR}.${CALAMARES_VERSION_MINOR}.${CALAMARES_VERSION_PATCH} )
set( CALAMARES_VERSION_SHORT "${CALAMARES_VERSION}" )
if( CALAMARES_VERSION_RC )
    set( CALAMARES_VERSION ${CALAMARES_VERSION}rc${CALAMARES_VERSION_RC} )
endif()

# additional info for non-release builds
if( NOT BUILD_RELEASE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git/" )
    include( CMakeDateStamp )
    set( CALAMARES_VERSION_DATE "${CMAKE_DATESTAMP_YEAR}${CMAKE_DATESTAMP_MONTH}${CMAKE_DATESTAMP_DAY}" )
    if( CALAMARES_VERSION_DATE GREATER 0 )
        set( CALAMARES_VERSION ${CALAMARES_VERSION}.${CALAMARES_VERSION_DATE} )
    endif()

    include( CMakeVersionSource )
    if( CMAKE_VERSION_SOURCE )
        set( CALAMARES_VERSION ${CALAMARES_VERSION}-${CMAKE_VERSION_SOURCE} )
    endif()
endif()

# enforce using constBegin, constEnd for const-iterators
add_definitions( "-DQT_STRICT_ITERATORS" )

# set paths
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}" )

# Better default installation paths: GNUInstallDirs defines
# CMAKE_INSTALL_FULL_SYSCONFDIR to be CMAKE_INSTALL_PREFIX/etc by default
# but we really want /etc
if( NOT DEFINED CMAKE_INSTALL_SYSCONFDIR )
    set( CMAKE_INSTALL_SYSCONFDIR "/etc" )
endif()

# make predefined install dirs available everywhere
include( GNUInstallDirs )

# make uninstall support
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

# Early configure these files as we need them later on
configure_file( CalamaresUse.cmake.in "${PROJECT_BINARY_DIR}/CalamaresUse.cmake" @ONLY )
file( COPY CalamaresAddLibrary.cmake DESTINATION "${PROJECT_BINARY_DIR}" )
file( COPY CalamaresAddModuleSubdirectory.cmake DESTINATION "${PROJECT_BINARY_DIR}" )
file( COPY CalamaresAddPlugin.cmake DESTINATION "${PROJECT_BINARY_DIR}" )
file( COPY CalamaresAddBrandingSubdirectory.cmake DESTINATION "${PROJECT_BINARY_DIR}" )

set( CALAMARES_LIBRARIES calamares )

add_subdirectory( src )

macro_display_feature_log()

if ( NOT WITH_PYTHON )
    message( "-- WARNING: Building Calamares without Python support. Python modules will not work.\n" )
endif()

# Add all targets to the build-tree export set
set( CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/Calamares" CACHE PATH  "Installation directory for CMake files" )
set( CMAKE_INSTALL_FULL_CMAKEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_CMAKEDIR}" )
export( TARGETS calamares
    FILE "${PROJECT_BINARY_DIR}/CalamaresLibraryDepends.cmake" )
    
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export( PACKAGE Calamares )

# Create a CalamaresBuildTreeSettings.cmake file for the use from the build tree
configure_file( CalamaresBuildTreeSettings.cmake.in "${PROJECT_BINARY_DIR}/CalamaresBuildTreeSettings.cmake" @ONLY )

# Create the CalamaresConfig.cmake and CalamaresConfigVersion files
file( RELATIVE_PATH CONF_REL_INCLUDE_DIR "${CMAKE_INSTALL_FULL_CMAKEDIR}" "${CMAKE_INSTALL_FULL_INCLUDEDIR}" )

configure_file( CalamaresConfig.cmake.in "${PROJECT_BINARY_DIR}/CalamaresConfig.cmake" @ONLY )
configure_file( CalamaresConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/CalamaresConfigVersion.cmake" @ONLY )

# Install the cmake files
install(
    FILES
        "${PROJECT_BINARY_DIR}/CalamaresConfig.cmake"
        "${PROJECT_BINARY_DIR}/CalamaresConfigVersion.cmake"
        "${PROJECT_BINARY_DIR}/CalamaresUse.cmake"
        "${PROJECT_BINARY_DIR}/CalamaresAddPlugin.cmake"
        "${PROJECT_BINARY_DIR}/CalamaresAddModuleSubdirectory.cmake"
        "${PROJECT_BINARY_DIR}/CalamaresAddLibrary.cmake"
        "${PROJECT_BINARY_DIR}/CalamaresAddBrandingSubdirectory.cmake"
    DESTINATION
        "${CMAKE_INSTALL_CMAKEDIR}"
)

# Install the export set for use with the install-tree
install(
    EXPORT
        CalamaresLibraryDepends
    DESTINATION
        "${CMAKE_INSTALL_CMAKEDIR}"
)

install(
    FILES
        settings.conf
    DESTINATION
        share/calamares
)

install(
  FILES
      com.github.calamares.calamares.policy
  DESTINATION
      "${POLKITQT-1_POLICY_FILES_INSTALL_DIR}"
)

install(
  FILES
      calamares.desktop
  DESTINATION
      ${CMAKE_INSTALL_DATADIR}/applications
)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
