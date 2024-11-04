cmake_minimum_required(VERSION 3.28.3)
project(TF C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")


if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Compilação em modo de depuração com suporte a profiling e sanitizers.")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg -fsanitize=address,undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -fsanitize=address,undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
endif()


# Use o Find_package para encontrar o SDL2 e SDL2_ttf nativos do Linux
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Adiciona o executável 'TF' usando o arquivo 'main.c'
add_executable(TF src/main.c)

# Ligar SDL2 e SDL2_ttf ao executável 'TF'
target_link_libraries(TF PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf)

# Incluir diretórios onde SDL2 está localizado
target_include_directories(TF PRIVATE ${SDL2_INCLUDE_DIRS})
