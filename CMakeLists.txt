cmake_minimum_required(VERSION 3.21)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  message(STATUS "Using ccache")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif(CCACHE_FOUND)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# On linux systems using clang, we need to link against libstdc++ explicitly
# https://cmake.org/pipermail/cmake/2006-January/008136.html
# http://www.reddit.com/r/cpp/comments/d1op8u/handling_libc_and_libstdc_in_cmake
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++ "
                    " -Wno-error=unused-command-line-argument")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi "
                           " -Wno-error=unused-command-line-argument")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Change this to the name of your plugin This cannot have spaces (but
# PRODUCT_NAME can)
set(PLUGIN_PROJECT_NAME "Wahmbulance")

set(CMAKE_BUILD_TYPE Release)

# Set the plugin formats you'll be building here. Valid formats: AAX Unity VST
# AU AUv3 Standalone
set(FORMATS Standalone VST3)

# Reads in VERSION file and sticks in it CURRENT_VERSION variable Be sure the
# file has no newlines
file(STRINGS VERSION CURRENT_VERSION)

# For simplicity, the name of the project is also the name of the target
project(
  ${PLUGIN_PROJECT_NAME}
  VERSION ${CURRENT_VERSION}
  LANGUAGES CXX C ASM OBJC)

# Enable to build universal binaries on macOS, increasing build time This only
# affects local builds, GitHub actions always builds Universals
set(CMAKE_OSX_ARCHITECTURES arm64 x86_64)
set(CMAKE_OSX_DEPLOYMENT_TARGET
    "10.13"
    CACHE STRING "Support macOS down to High Sierra")

# JUCE is setup as a submodule in the /JUCE folder Locally, you'll need to run
# `git submodule update --init --recursive` once and `git submodule update
# --remote --merge` to keep it up to date On Github Actions, it's managed by
# actions/checkout
add_subdirectory(libs/JUCE EXCLUDE_FROM_ALL)

# Check the readme at `docs/CMake API.md` in the JUCE repo for full config
juce_add_plugin(
  "${PLUGIN_PROJECT_NAME}"
  COMPANY_NAME
  cwoodall
  BUNDLE_ID
  com.cwoodall.wahmbulance
  # On MacOS, plugin will be copied to /Users/you/Library/Audio/Plug-Ins/
  COPY_PLUGIN_AFTER_BUILD
  TRUE
  # This has to be one uppercase, rest lower for AU formats
  PLUGIN_MANUFACTURER_CODE
  Chri
  # A unique four-character plugin id with at least one upper-case character
  PLUGIN_CODE
  P001
  FORMATS
  "${FORMATS}"
  PRODUCT_NAME
  "${PLUGIN_PROJECT_NAME}")

add_subdirectory(libs/CMP EXCLUDE_FROM_ALL)

# Manually list all .h and .cpp files for the plugin
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h")

target_sources("${PLUGIN_PROJECT_NAME}" PRIVATE ${SOURCE_FILES})

# Setup our binary data as a target
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/resources)

# Unfortunately, cmp_plot does not set its headers as SYSTEM, so we get errors
# with our warning level. So we will need to ignore them
get_target_property(_inc cmp_plot INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(cmp_plot SYSTEM INTERFACE ${_inc})

# We'll need to link to these from our plugin as well as our tests
add_library(juce_lib STATIC)
target_link_libraries(
  juce_lib
  PUBLIC juce::juce_core juce::juce_audio_utils juce::juce_audio_devices
         juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
         juce::juce_recommended_warning_flags)

target_compile_definitions(
  "${PLUGIN_PROJECT_NAME}"
  PRIVATE # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you
          # might not need them.
          JUCE_WEB_BROWSER=0 # If you remove this, add `NEEDS_WEB_BROWSER TRUE`
          # to the `juce_add_plugin` call
          JUCE_USE_CURL=0 # If you remove this, add `NEEDS_CURL TRUE` to the
                          # `juce_add_plugin` call
          JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries("${PLUGIN_PROJECT_NAME}" PRIVATE juce_lib cmp_plot Assets)

# Required for ctest (which is just easier for cross-platform CI) include(CTest)
# does this too, but adds tons of targets we don't want See:
# https://github.com/catchorg/Catch2/issues/2026 You could forgo ctest and call
# ./tests directly from the build dir
enable_testing()

file(GLOB_RECURSE TestFiles CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h")
# Use Catch2 v3 on the devel branch
add_subdirectory(libs/Catch2 EXCLUDE_FROM_ALL)

add_executable(tests ${TestFiles})
target_compile_features(tests PRIVATE cxx_std_20)
target_compile_definitions(tests PRIVATE CATCH_CONFIG_FAST_COMPILE)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/source)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain
                                    "${PLUGIN_PROJECT_NAME}" juce_lib)

set_target_properties(Catch2 PROPERTIES UNITY_BUILD ON)

# Load and use the .cmake file provided by Catch2
# https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md We
# have to manually provide the source directory here for now
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(tests)
