cmake_minimum_required(VERSION 3.15)
project(CERN_Tymon_Tumialis CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. Włączenie Link-Time Optimization (LTO) dla maksymalnej wydajności (optymalizacja dla 100k zdarzeń)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()



# 3. Wyszukiwanie pakietu ROOT
find_package(ROOT REQUIRED COMPONENTS Core Hist RIO)

# --- 4. AUTOMATYCZNE WYKRYWANIE PYTHIA 8 ---
# Szukamy narzędzia pythia8-config w systemie lub w ścieżce podanej przez użytkownika
find_program(PYTHIA_CONFIG pythia8-config 
    HINTS $ENV{PYTHIA_PATH}/bin /usr/bin /usr/local/bin /opt/pythia8/bin
)

if(NOT VDT_FOUND)
    # Tworzenie atrapy nagłówka wersji
    file(WRITE "${CMAKE_BINARY_DIR}/fake_vdt/vdt/vdtMath.h" "#define VDT_VERSION 0.4.4\n")
    set(VDT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/fake_vdt" CACHE PATH "Fake VDT include" FORCE)
    
    # Dynamiczne znalezienie systemowej biblioteki 'dl' (zamiast sztywnego /usr/lib/...)
    find_library(SYSTEM_DL_LIB NAMES dl)
    if(SYSTEM_DL_LIB)
        set(VDT_LIBRARY ${SYSTEM_DL_LIB} CACHE FILEPATH "Fake VDT library" FORCE)
    endif()
    
    set(VDT_FOUND TRUE CACHE BOOL "Fake VDT found" FORCE)
    set(Vdt_FOUND TRUE CACHE BOOL "Fake VDT found" FORCE)
endif()

if(NOT PYTHIA_CONFIG)
    message(FATAL_ERROR "Nie znaleziono pythia8-config! Upewnij się, że Pythia 8 jest zainstalowana i dodana do zmiennej PATH, lub ustaw zmienną środowiskową PYTHIA_PATH (np. export PYTHIA_PATH=/sciezka/do/pythia) przed uruchomieniem CMake.")
endif()

# Odpytanie pythia8-config o ścieżki do nagłówków, bibliotek i plików XML
execute_process(COMMAND ${PYTHIA_CONFIG} --includedir OUTPUT_VARIABLE PYTHIA_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${PYTHIA_CONFIG} --libdir OUTPUT_VARIABLE PYTHIA_LIB_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${PYTHIA_CONFIG} --xmldoc OUTPUT_VARIABLE PYTHIA_XML_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "Pythia 8 zlokalizowana pomyślnie:")
message(STATUS " - Nagłówki: ${PYTHIA_INCLUDE_DIR}")
message(STATUS " - Biblioteki: ${PYTHIA_LIB_DIR}")
message(STATUS " - Pliki XML: ${PYTHIA_XML_DIR}")

# 5. Automatyczne zbieranie źródeł C++
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 6. Utworzenie celu wykonywalnego
add_executable(app main.cpp ${SOURCES})

# Przekazanie ścieżki XML do kodu C++ jako makro preprocesora
target_compile_definitions(app PRIVATE PYTHIA_XML_DIR="${PYTHIA_XML_DIR}")

# 7. Flagi kompilatora
target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)
target_compile_options(app PRIVATE
    $<$<CONFIG:Debug>:-O0 -Wshadow -Wall -fsanitize=address,leak,undefined,float-divide-by-zero,float-cast-overflow -g -fsignaling-nans -fnon-call-exceptions>
    $<$<CONFIG:Release>:-O3 >
)

# 8. Ścieżki nagłówkowe
target_include_directories(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PYTHIA_INCLUDE_DIR}
    ${ROOT_INCLUDE_DIRS}
)

# 9. Linkowanie Pythii 8 oraz ROOT-a
target_link_directories(app PRIVATE ${PYTHIA_LIB_DIR})
target_link_libraries(app PRIVATE 
    pythia8 
    dl 
    ROOT::Core 
    ROOT::Hist 
    ROOT::RIO
)

target_link_options(app PRIVATE
        $<$<CONFIG:Debug>:-O0 -Wshadow -Wall -fsanitize=address,leak,undefined,float-divide-by-zero,float-cast-overflow -g -fsignaling-nans -fnon-call-exceptions>
)