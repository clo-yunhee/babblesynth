add_executable(babblesynth-cli EXCLUDE_FROM_ALL
    main.cpp
)

target_link_libraries(babblesynth-cli PRIVATE babblesynth dr_libs)
