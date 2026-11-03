cmake_minimum_required(VERSION 3.15)
project(CERN_Tymon_Tumialis CXX)

# 1. Ustawienie domyślnego trybu budowania na Release (jeśli nie wybrano inaczej)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Wybierz tryb budowania: Debug lub Release" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYTHIA_PATH "/home/tymon/coding/Pythia/pythia8318") 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(app main.cpp ${SOURCES})

# 2. Globalne flagi kompilatora (ostrzeżenia, polecane zawsze)
target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)

# 3. Flagi specyficzne dla konkretnego trybu (Modern CMake)
target_compile_options(app PRIVATE
    # Tryb Debug: wyłączenie optymalizacji (-O0), maksymalne informacje do debuggera (-g3)
    $<$<CONFIG:Debug>:-O0 -DDEBUG -Wshadow -Wall -fsanitize=address,leak,undefined,float-divide-by-zero,float-cast-overflow -g -fsignaling-nans -fnon-call-exceptions -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC >
    
    # Tryb Release: maksymalna optymalizacja (-O3), optymalizacja pod procesor (-march=native)
    $<$<CONFIG:Release>:-O3 -march=native>
)

target_include_directories(app PRIVATE ${PYTHIA_PATH}/include)
target_link_directories(app PRIVATE ${PYTHIA_PATH}/lib)
target_link_libraries(app PRIVATE pythia8 dl)