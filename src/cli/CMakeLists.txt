add_executable(babblesynth-cli 
    main.cpp
)

target_link_libraries(babblesynth-cli PRIVATE babblesynth dr_libs)
