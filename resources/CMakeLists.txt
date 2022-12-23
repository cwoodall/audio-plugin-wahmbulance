# 3.21 is bundled in VSCode 2022
# https://docs.microsoft.com/en-us/visualstudio/releases/2022/release-notes
# 3.21 is also bundled in CLion as of 2021.3
cmake_minimum_required(VERSION 3.21)

# project(AutoWah_Resources)

set(Python3_FIND_UNVERSIONED_NAMES NEVER)
find_package(Python3 REQUIRED)
if(NOT Python3_Interpreter_FOUND)
    message(FATAL_ERROR "Python3 is needed, please install it first")
endif()

# Execute the generation of the knob.svg into underlying layers
execute_process(
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/svglayers2svg.py knob.svg
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

file(GLOB KnobFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/knob/knob_*.svg")
file(GLOB BackgroundFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/background/*.png")
file(GLOB DebugFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/debug/*.png")

juce_add_binary_data(Assets SOURCES 
    ${KnobFiles}
    ${BackgroundFiles}
    ${DebugFiles}
    )

# Required for Linux happiness:
# See https://forum.juce.com/t/loading-pytorch-model-using-binarydata/39997/2
set_target_properties(Assets PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
