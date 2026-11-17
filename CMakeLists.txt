cmake_minimum_required(VERSION 3.14)
project(MojProjekt CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Wyszukanie wxWidgets (musi być PRZED użyciem jej zmiennych)
find_package(wxWidgets REQUIRED COMPONENTS core base)

# Wymagane przez wxWidgets w CMake do poprawnego ustawienia flag i ścieżek
include(${wxWidgets_USE_FILE})

# 2. Automatyczne pobranie wszystkich plików .cpp i .h z podfolderów
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.h")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "include/*.h" "include/*.hpp")

# (Opcjonalnie) Pobranie plików .fbp
file(GLOB FBP_FILES CONFIGURE_DEPENDS "*.fbp") 

# 3. Utworzenie pliku wykonywalnego ze wszystkich znalezionych plików
add_executable(MojProjekt 
    ${SOURCES} 
    ${HEADERS}
    ${FBP_FILES}
)

# 4. POWIĄZANIE Z WXWIDGETS (To naprawia błąd z brakiem pliku <wx/artprov.h>)
target_link_libraries(MojProjekt PRIVATE ${wxWidgets_LIBRARIES})

if(MSVC)
    target_compile_options(MojProjekt PRIVATE /utf-8)
endif()

# 5. Wskazanie kompilatorowi, gdzie szukać Twoich własnych plików nagłówkowych
target_include_directories(MojProjekt PRIVATE 
    include
    src
)