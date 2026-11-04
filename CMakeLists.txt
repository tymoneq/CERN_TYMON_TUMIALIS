cmake_minimum_required(VERSION 3.15)
project(CERN_Tymon_Tumialis CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PYTHIA_PATH "/home/tymon/coding/Pythia/pythia8318") 

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. Włączenie Link-Time Optimization (LTO) dla maksymalnej wydajności (optymalizacja dla 100k zdarzeń)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# 2. Tworzenie atrapy pliku vdtMath.h, aby oszukać skrypt spradzający wersję FindVdt.cmake
file(WRITE "${CMAKE_BINARY_DIR}/fake_vdt/vdt/vdtMath.h" "#define VDT_VERSION 0.4.4\n")
set(VDT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/fake_vdt" CACHE PATH "" FORCE)
set(VDT_LIBRARY "/usr/lib/libdl.so" CACHE FILEPATH "" FORCE)
set(VDT_FOUND TRUE CACHE BOOL "" FORCE)
set(Vdt_FOUND TRUE CACHE BOOL "" FORCE)

# 3. Wyszukiwanie pakietu ROOT
find_package(ROOT REQUIRED COMPONENTS Core Hist RIO)

# --- NOWE: Automatyczne pobranie ścieżki XML z Pythii ---
execute_process(
    COMMAND ${PYTHIA_PATH}/bin/pythia8-config --xmldoc
    OUTPUT_VARIABLE PYTHIA_XML_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 4. Automatyczne zbieranie źródeł C++
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 5. Utworzenie celu wykonywalnego
add_executable(app main.cpp ${SOURCES})

# --- NOWE: Przekazanie ścieżki XML do kodu C++ jako makro preprocesora ---
target_compile_definitions(app PRIVATE PYTHIA_XML_DIR="${PYTHIA_XML_DIR}")

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