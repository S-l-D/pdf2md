cmake_minimum_required(VERSION 3.25)

set(VERSION_MAJOR 0 CACHE STRING "Major version")
set(VERSION_MINOR 1 CACHE STRING "Minor version")
set(VERSION_BUILD 0 CACHE STRING "Build version")
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD}" CACHE STRING "Full version" FORCE)

project(pdf2md_engine VERSION ${VERSION} LANGUAGES CXX)

set(PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# --- Git metadata ---
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY "${PROJECT_ROOT_DIR}"
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(GIT_BRANCH "unknown")
endif()

# --- Опции ---
option(TESTS_ENABLED "Включение сборки тестов" ${PROJECT_IS_TOP_LEVEL})
option(ENABLE_CLANG_TIDY "Запускать clang-tidy при сборке" ON)
option(ENABLE_AVX2 "Включить AVX2 оптимизации в Release" ON)


# --- Конфигурация путей вывода ---
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" LOWERCASE_BUILD_TYPE)
set(OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/output/${LOWERCASE_BUILD_TYPE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib/static")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}/lib/shared")

# --- Стандарт и флаги компиляции ---
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(FATAL_ERROR "This project requires Clang, found: ${CMAKE_CXX_COMPILER_ID}")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_compile_options(
    -Wall -Wextra -Werror -Wpedantic
    -Wshadow -Wconversion -Wsign-conversion
    -fcolor-diagnostics # Цветной вывод при сборке через Ninja
)
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND ENABLE_AVX2)
    add_compile_options(-O3 -mavx2 -mfma)
endif()
# Опционально ускоряем линковку через lld, если он есть в системе
find_program(LLD_LINKER NAMES "lld" "ld.lld")
if(LLD_LINKER)
    add_link_options(-fuse-ld=lld)
endif()

# --- Clang-tidy ---
if(ENABLE_CLANG_TIDY)
string(REGEX MATCH "^[0-9]+" CLANG_MAJOR "${CMAKE_CXX_COMPILER_VERSION}")
find_program(CLANG_TIDY NAMES "clang-tidy-${CLANG_MAJOR}" "clang-tidy")
    if(CLANG_TIDY)
        message(STATUS "Found clang-tidy: ${CLANG_TIDY}")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}")
    elseif(PROJECT_IS_TOP_LEVEL)
        message(FATAL_ERROR "clang-tidy not found!")
    endif()
endif()

# --- Зависимости из Conan (CMakeDeps) ---
find_package(WebP CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)

if(TESTS_ENABLED)
    find_package(GTest CONFIG REQUIRED)
    include(GoogleTest)
    enable_testing()
endif()

# --- Предсобранный PDFium ---
set(PDFIUM_DIR "${PROJECT_ROOT_DIR}/source/pdfium")

if(NOT EXISTS "${PDFIUM_DIR}/lib/libpdfium.so")
    message(FATAL_ERROR "PDFium library not found at ${PDFIUM_DIR}/lib/libpdfium.so")
endif()

add_library(pdfium::pdfium SHARED IMPORTED)
set_target_properties(pdfium::pdfium PROPERTIES
    IMPORTED_LOCATION "${PDFIUM_DIR}/lib/libpdfium.so"
    INTERFACE_INCLUDE_DIRECTORIES "${PDFIUM_DIR}/include"
)
# Помечаем заголовки PDFium как SYSTEM, чтобы -Werror не ругался на C-код внутри
set_target_properties(pdfium::pdfium PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${PDFIUM_DIR}/include"
)

# --- Предсобранный ONNX Runtime (CPU) ---
set(ORT_DIR "${PROJECT_ROOT_DIR}/source/ORT")

if(NOT EXISTS "${ORT_DIR}/lib/libonnxruntime.so")
    message(FATAL_ERROR "ONNX Runtime library not found at ${ORT_DIR}/lib/libonnxruntime.so")
endif()

add_library(onnxruntime::onnxruntime SHARED IMPORTED)
set_target_properties(onnxruntime::onnxruntime PROPERTIES
    IMPORTED_LOCATION "${ORT_DIR}/lib/libonnxruntime.so"
    INTERFACE_INCLUDE_DIRECTORIES "${ORT_DIR}/include"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${ORT_DIR}/include"
)

message(STATUS "*==============================*")
message(STATUS "| Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "| Generator  : ${CMAKE_GENERATOR}")
message(STATUS "| Compiler   : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "| Git Branch : ${GIT_BRANCH}")
message(STATUS "| Version    : ${VERSION}")
message(STATUS "*==============================*")

add_subdirectory(src)
if(TESTS_ENABLED)
    add_subdirectory(tests)
endif()