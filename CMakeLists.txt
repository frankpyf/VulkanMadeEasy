cmake_minimum_required(VERSION 3.25)

project(
    VulkanMadeEasy
    VERSION 0.1.0
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)
find_package(Vulkan REQUIRED)
file(GLOB SRC src/*.cpp)
file(GLOB HDS include/*.h)

# glfw
add_subdirectory(external/glfw)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_library(VulkanMadeEasy SHARED ${SRC})
target_sources(VulkanMadeEasy
        PRIVATE
        FILE_SET vme_headers
        TYPE HEADERS
        FILES ${HDS})
        
target_link_libraries(VulkanMadeEasy PUBLIC ${Vulkan_LIBRARIES} glfw)
target_include_directories(VulkanMadeEasy PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(VulkanMadeEasy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)