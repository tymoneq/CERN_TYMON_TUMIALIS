cmake_minimum_required(VERSION 3.15)
project(CERN_Tymon_Tumialis CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 1. Standard C++20 (wymagany przez ROOT)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYTHIA_PATH "/home/tymon/coding/Pythia/pythia8318") 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 2. Tworzenie atrapy pliku vdtMath.h, aby oszukać skrypt spradzający wersję FindVdt.cmake
file(WRITE "${CMAKE_BINARY_DIR}/fake_vdt/vdt/vdtMath.h" "#define VDT_VERSION 0.4.4\n")
set(VDT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/fake_vdt" CACHE PATH "" FORCE)
set(VDT_LIBRARY "/usr/lib/libdl.so" CACHE FILEPATH "" FORCE)
set(VDT_FOUND TRUE CACHE BOOL "" FORCE)
set(Vdt_FOUND TRUE CACHE BOOL "" FORCE)

# 3. Wyszukiwanie pakietu ROOT
find_package(ROOT REQUIRED COMPONENTS Core Hist RIO)

# 4. Automatyczne zbieranie źródeł C++
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 5. Utworzenie celu wykonywalnego
add_executable(app main.cpp ${SOURCES})

# 6. Flagi kompilatora
target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(app PRIVATE
    $<$<CONFIG:Debug>:-O0 -g3 -DDEBUG>
    $<$<CONFIG:Release>:-O3 -march=native>
)

# 7. Ścieżki nagłówkowe
target_include_directories(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PYTHIA_PATH}/include
    ${ROOT_INCLUDE_DIRS}
)

# 8. Linkowanie Pythii 8 oraz ROOT-a
target_link_directories(app PRIVATE ${PYTHIA_PATH}/lib)
target_link_libraries(app PRIVATE 
    pythia8 
    dl 
    ROOT::Core 
    ROOT::Hist 
    ROOT::RIO
)