cmake_minimum_required(VERSION 3.25)

project(
    VulkanMadeEasy
    VERSION 0.1.0
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)

# spdlog
add_subdirectory(external/spdlog)

# Vulkan
find_package(Vulkan REQUIRED)

# VMA
add_subdirectory(external/vma)

# glfw
add_subdirectory(external/glfw)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_library(VulkanMadeEasy SHARED)
target_sources(VulkanMadeEasy PRIVATE
    include/VulkanEngine.h
    include/Window.h
    src/VulkanEngine.cpp
    src/VulkanMemoryAllocator.cpp
    src/Window.cpp
)

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(BUILD_SHARED_LIBS TRUE)
endif()
        
target_link_libraries(
    VulkanMadeEasy 
    PUBLIC ${Vulkan_LIBRARIES} glfw GPUOpen::VulkanMemoryAllocator
    PRIVATE spdlog::spdlog
)
target_include_directories(VulkanMadeEasy PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(VulkanMadeEasy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)