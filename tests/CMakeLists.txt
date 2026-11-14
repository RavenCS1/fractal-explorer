# Testy bez okna. Uruchamianie: ctest --test-dir build --output-on-failure
# Kazdy moze dopisac wlasny plik testow (np. ifs_tests.cpp) i linkowac swoj modul.

add_executable(core_tests core_tests.cpp)
target_link_libraries(core_tests PRIVATE fe_core)
fe_setup_target(core_tests)
add_test(NAME core_tests COMMAND core_tests)
