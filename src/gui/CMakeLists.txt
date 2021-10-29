set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets Multimedia Charts LinguistTools REQUIRED)

set(TS_FILES translations/babblesynth-gui_en_GB.ts)

qt5_add_big_resources(BIG_QRC_FILES
    fonts/fonts.qrc
    icons/icons.qrc
    qdarkstyle/dark/style.qrc
)

add_executable(babblesynth-gui WIN32
    app_state.cpp
    app_state.h
    app_window.cpp
    app_window.h
    audio_player.cpp
    audio_player.h
    chart_view.cpp
    chart_view.h
    clickable_label.cpp
    clickable_label.h
    filter_tracks.cpp
    filter_tracks.h
    source_parameters.cpp
    source_parameters.h
    source_plan_item.cpp
    source_plan_item.h
    source_plan.cpp
    source_plan.h
    colorplot/coloraxis/chartcoloraxisx.cpp
    colorplot/coloraxis/chartcoloraxisy.cpp
    colorplot/coloraxis/coloraxis.cpp
    colorplot/coloraxis/qcoloraxis.cpp
    colorplot/colorlineseries/colorlinechartitem.cpp
    colorplot/colorlineseries/qcolorlineseries.cpp
    colorplot/colorxyseries/colorxychart.cpp
    colorplot/colorxyseries/qcolorxyseries.cpp
    main.cpp
    ${TS_FILES}
    ${BIG_QRC_FILES}
)

qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

target_compile_definitions(babblesynth-gui PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
    _UNICODE
)

target_link_libraries(babblesynth-gui PRIVATE
    Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Charts Qt5::ChartsPrivate
    babblesynth
)

set_target_properties(babblesynth-gui PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER fr.cloyunhee.babblesynth
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

add_custom_command(TARGET babblesynth-gui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -DTARGET_EXECUTABLE=$<TARGET_FILE:babblesynth-gui> -DQt5_DIR=${Qt5_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/../../copy_runtime_deps.cmake
)
