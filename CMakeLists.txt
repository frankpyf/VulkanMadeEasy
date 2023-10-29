cmake_minimum_required(VERSION 3.25)

project(
    VulkanMadeEasy
    VERSION 0.1.0
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 20)
find_package(Vulkan REQUIRED)
file(GLOB SRC src/*.cpp)

add_library(VulkanMadeEasy SHARED ${SRC})
target_link_libraries(VulkanMadeEasy PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(VulkanMadeEasy PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(VulkanMadeEasy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(GNUInstallDirs)
install(TARGETS VulkanMadeEasy
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
