set(LICENSE_DESTINATION .)
set(BIN_DESTINATION .)
set(LIB_DESTINATION .)
set(PLATFORM_PLUGINS qwindows)
set(AUDIO_PLUGINS wasapi windows)

set(RUNTIME_DEPS TRUE)
set(RUNTIME_DEPS_PRE_EX "api-ms-*" "ext-ms-*")
set(RUNTIME_DEPS_POST_EX ".*system32/.*\\.dll")
set(RUNTIME_DEPS_DIRS ${QT_ROOT}/bin)

set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall BabbleSynth")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/branding/icon.ico")
set(CPACK_NSIS_MUI_FINISHPAGE_RUN "..\\\\babblesynth-gui.exe")
set(CPACK_PACKAGE_EXECUTABLES "..\\\\babblesynth-gui;BabbleSynth")