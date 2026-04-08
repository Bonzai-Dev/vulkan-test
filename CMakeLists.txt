cmake_minimum_required(VERSION 4.0)
project(vulkan-test)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)

# Set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/generators/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/generators/$<CONFIGURATION>")

# Creating executable
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "src/*.h" "src/*.c" "src/*.hpp" "src/*.cpp")
add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Libraries
add_subdirectory(thirdparty)