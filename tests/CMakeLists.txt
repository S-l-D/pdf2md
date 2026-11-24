add_executable(pdf2md_tests
    smoke_test.cpp
)

target_link_libraries(pdf2md_tests
    PRIVATE
        pdf2md_core
        GTest::gtest_main
)

# Отключаем clang-tidy для тестов, чтобы макросы GTest не вызывали ложных срабатываний
set_target_properties(pdf2md_tests PROPERTIES CXX_CLANG_TIDY "")

gtest_discover_tests(pdf2md_tests)
