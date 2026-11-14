# Headless tests. Run with: ctest --test-dir build --output-on-failure
# Everyone can add their own test file (e.g. ifs_tests.cpp) and link their module.

add_executable(core_tests core_tests.cpp)
target_link_libraries(core_tests PRIVATE fe_core)
fe_setup_target(core_tests)
add_test(NAME core_tests COMMAND core_tests)
