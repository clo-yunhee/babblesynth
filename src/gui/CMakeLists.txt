set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Multimedia Charts PrintSupport LinguistTools REQUIRED)

set(TS_FILES translations/babblesynth-gui_en_GB.ts)

add_executable(babblesynth-gui WIN32
    app_state.cpp
    app_state.h
    audio_player.cpp
    audio_player.h
    audio_writer.cpp
    audio_writer.h
    widgets/app_window.cpp
    widgets/app_window.h
    widgets/clickable_label.cpp
    widgets/clickable_label.h
    widgets/source_parameters.cpp
    widgets/source_parameters.h
    phonemes/pole_zero.h
    phonemes/phoneme_dictionary.cpp
    phonemes/phoneme_dictionary.h
    phonemes/phoneme.cpp
    phonemes/phoneme.h
    qcustomplot/qcustomplot.cpp
    qcustomplot/qcustomplot.h
    voicefxtype/animal_crossing.cpp
    voicefxtype/animal_crossing.h
    voicefxtype/undertale.cpp
    voicefxtype/undertale.h
    voicefxtype/voicefxtype.h
    main.cpp
    dictionaries/dictionaries.qrc
    fonts/fonts.qrc
    icons/icons.qrc
    qdarkstyle/dark/style.qrc
    ${TS_FILES}
    $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Windows>:icons/appicon.rc>
)

qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

target_link_libraries(babblesynth-gui PRIVATE
    Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Charts Qt5::ChartsPrivate Qt5::PrintSupport
    babblesynth fftw3 xerces-c sndfile
)

set_target_properties(babblesynth-gui PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER fr.cloyunhee.babblesynth
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

add_custom_command(TARGET babblesynth-gui POST_BUILD
    COMMAND ${CMAKE_COMMAND}
                -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DCMAKE_CROSSCOMPILING=${CMAKE_CROSSCOMPILING}
                -DTARGET_EXECUTABLE=$<TARGET_FILE:babblesynth-gui>
                -DFFTW_OUTPUT_DIRECTORY=$<TARGET_FILE_DIR:fftw3>
                -DQT_RUNTIME_DIR=${QT_RUNTIME_DIR}
                -DQT_LIBRARY_DIR=${QT_LIBRARY_DIR}
                -DQT_PLUGIN_DIR=${QT_PLUGIN_DIR}
                -P ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/copy_runtime_deps.cmake
)
