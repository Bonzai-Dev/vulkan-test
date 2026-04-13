# Set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

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
find_package(Vulkan REQUIRED)
target_compile_definitions(${PROJECT_NAME} PRIVATE VK_NO_PROTOTYPES)
target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Vulkan::Vulkan Vulkan::Headers)
