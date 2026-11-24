#include "pdf_reader.hpp"

#include <CLI/CLI.hpp>
#include <exception>
#include <filesystem>
#include <fmt/core.h>

int main(int argc, char** argv) {
    CLI::App app{"PDF to Markdown engine"};

    std::filesystem::path input_pdf;
    app.add_option("-i,--input", input_pdf, "Path to input PDF file")
        ->required()
        ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    try {
        const pdf2md::PdfReader reader;
        const int page_count = reader.getPageCount(input_pdf);
        fmt::print("Loaded '{}'. Pages: {}\n", input_pdf.string(), page_count);
    } catch (const std::exception& e) {
        fmt::print(stderr, "Error: {}\n", e.what());
        return 1;
    }

    return 0;
}
