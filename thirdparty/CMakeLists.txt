# Set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Slang
set(SLANG_ENABLE_PCH OFF CACHE BOOL "" FORCE)
set(SLANG_LIB_TYPE "STATIC" CACHE STRING "" FORCE)
set(SLANG_ENABLE_TESTS OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SLANG_ENABLE_GFX OFF CACHE BOOL "" FORCE)
add_subdirectory(slang)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE slang)

# SDL
add_subdirectory(SDL EXCLUDE_FROM_ALL)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE SDL3::SDL3)

# spdlog
add_subdirectory(spdlog)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE spdlog)

# glm
add_subdirectory(glm)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glm::glm)

# Vulkan
find_path(VULKAN_INCLUDE_DIRECTORY NAMES vulkan/vulkan.h PATHS "$ENV{VULKAN_SDK}/include" NO_DEFAULT_PATH)
find_library(VULKAN_LIBRARY NAMES vulkan PATHS "$ENV{VULKAN_SDK}/lib" NO_DEFAULT_PATH)
if (VULKAN_LIBRARY AND VULKAN_INCLUDE_DIRECTORY)
    message(STATUS "Found Vulkan SDK at ${VULKAN_LIBRARY}")
else ()
    message(STATUS "Vulkan SDK not found")
endif ()

add_library(Vulkan::Vulkan UNKNOWN IMPORTED)
set_target_properties(
    Vulkan::Vulkan PROPERTIES IMPORTED_LOCATION "${VULKAN_LIBRARY}" INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_INCLUDE_DIRECTORY}"
)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Vulkan::Vulkan)
