add_library(pdf2md_core STATIC
    pdf_reader.cpp
)

target_include_directories(pdf2md_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(pdf2md_core
    PUBLIC
        pdfium::pdfium
    PRIVATE
        fmt::fmt
        onnxruntime::onnxruntime
        WebP::webp
        stb::stb
)

add_executable(pdf2md_engine
    main.cpp
)

target_link_libraries(pdf2md_engine
    PRIVATE
        pdf2md_core
        CLI11::CLI11
        fmt::fmt
)
