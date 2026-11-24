#include "pdf_reader.hpp"

#include <fpdfview.h>
#include <stdexcept>
#include <string>

namespace pdf2md {

PdfReader::PdfReader() {
    FPDF_InitLibrary();
}

PdfReader::~PdfReader() {
    FPDF_DestroyLibrary();
}

int PdfReader::getPageCount(const std::filesystem::path& pdf_path) const {
    FPDF_DOCUMENT doc = FPDF_LoadDocument(pdf_path.c_str(), nullptr);
    if (doc == nullptr) {
        throw std::runtime_error("Failed to open PDF: " + pdf_path.string());
    }
    const int pages = FPDF_GetPageCount(doc);
    FPDF_CloseDocument(doc);
    return pages;
}

} // namespace pdf2md
