# 3.21 is bundled in VSCode 2022
# https://docs.microsoft.com/en-us/visualstudio/Releases/2022/release-notes 3.21
# is also bundled in CLion as of 2021.3
cmake_minimum_required(VERSION 3.21)

# cmake-lint: disable=C0103
find_program(POETRY poetry)
if(POETRY)
  if(NOT EXISTS ${PROJECT_BINARY_DIR}/pyproject.toml)
    configure_file(${PROJECT_SOURCE_DIR}/pyproject.toml
                   ${PROJECT_BINARY_DIR}/pyproject.toml)
  endif()

  if(NOT EXISTS ${PROJECT_BINARY_DIR}/poetry.toml)
    configure_file(${PROJECT_SOURCE_DIR}/poetry.toml
                   ${PROJECT_BINARY_DIR}/poetry.toml)
  endif()

  if(NOT EXISTS ${PROJECT_BINARY_DIR}/.venv)
    execute_process(COMMAND ${_CMD_COMMAND} poetry install
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  endif()
  set(Python3_ROOT_DIR ${PROJECT_BINARY_DIR}/.venv)
  set(Python3_EXECUTABLE ${Python3_ROOT_DIR}/bin/python)
  if(MSVC)
    set(Python3_EXECUTABLE ${Python3_ROOT_DIR}/Scripts/python.exe)
  endif()
  file(WRITE ${PROJECT_BINARY_DIR}/.python_packages "")
else()
  message(
    FATAL_ERROR "Poetry is required. Please install poetry. CMake will exit.")
endif()

# Execute the generation of the knob.svg into underlying layers
execute_process(
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_LIST_DIR}/svglayers2svg.py
          knob.svg WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

file(GLOB KnobFiles CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_LIST_DIR}/knob/knob_*.svg")
file(GLOB BackgroundFiles CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_LIST_DIR}/background/*.png")
file(GLOB DebugFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/debug/*.png")

juce_add_binary_data(
  Assets SOURCES ${KnobFiles} ${BackgroundFiles} ${DebugFiles}
  ${CMAKE_CURRENT_LIST_DIR}/FiraMono-Regular.ttf)

# Required for Linux happiness: See
# https://forum.juce.com/t/loading-pytorch-model-using-binarydata/39997/2
set_target_properties(Assets PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
