cmake_minimum_required(VERSION 3.10)
project(Voxel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

if(MSVC)
  foreach(flag_var CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
    string(REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
  endforeach()
endif()

include_directories(${CMAKE_SOURCE_DIR}/external/include)

link_directories(${CMAKE_SOURCE_DIR}/external/lib)

file(GLOB_RECURSE PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE UTILS_SOURCES ${CMAKE_SOURCE_DIR}/src/utils/*.cpp)

set(PROJECT_SOURCES ${PROJECT_SOURCES} ${UTILS_SOURCES})


set(GLAD_SOURCE ${CMAKE_SOURCE_DIR}/external/include/glad/glad.c)
if(EXISTS ${GLAD_SOURCE})
  list(APPEND PROJECT_SOURCES ${GLAD_SOURCE})
  set_source_files_properties(${GLAD_SOURCE} PROPERTIES LANGUAGE C)
else()
  message(WARNING "GLAD source file not found at ${GLAD_SOURCE}")
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}  )

target_link_libraries(${PROJECT_NAME}
  opengl32.lib
  glfw3.lib
  gdi32.lib
  user32.lib
  shell32.lib
  kernel32.lib
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:DEBUG>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
  COMMENT "Copying assets (with gfx and shaders subfolders) to the output folder..."
)
