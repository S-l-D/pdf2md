#pragma once

#include <filesystem>

namespace pdf2md {

class PdfReader {
public:
    PdfReader();
    ~PdfReader();

    PdfReader(const PdfReader&) = delete;
    PdfReader& operator=(const PdfReader&) = delete;
    PdfReader(PdfReader&&) = delete;
    PdfReader& operator=(PdfReader&&) = delete;

    [[nodiscard]] int getPageCount(const std::filesystem::path& pdf_path) const;
};

} // namespace pdf2md
