add_executable(Step
  anglelabel.cpp anglelabel.h
  common_panel_controls.cpp
  initRunNextTrial.cpp
  main.cpp main.h
  panelab5.cpp panelab5.h
  panelab7.cpp panelab7.h
  paneladmin.cpp paneladmin.h
  panelbs1116.cpp panelbs1116.h
  panelmushra.cpp panelmushra.h
  qt_interface.cpp qt_interface.h
  qt_message.cpp qt_message.h
  
  panelab5.ui
  panelab7.ui
  paneladmin.ui
  panelbs1116.ui
  panelmushra.ui
  
  icons8-music-1024.icns
)
target_link_libraries(Step step_port Qt6::Core Qt6::Gui Qt6::Widgets)

# These files are included by each panel*.cpp files so we do not compile them
set_source_files_properties(initRunNextTrial.cpp common_panel_controls.cpp
  PROPERTIES HEADER_FILE_ONLY TRUE)
  
set(RESOURCE_FILES icons8-music-1024.icns)

if (APPLE)
  set_target_properties(Step PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.AudioResearchLabs.step"
    MACOSX_BUNDLE_ICON_FILE icons8-music-1024.icns
    MACOSX_BUNDLE_LONG_VERSION_STRING ${SHORT_VERSION}.${VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${SHORT_VERSION}.${VERSION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION}
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2004, 2023 Audio Research Labs, LLC"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    RESOURCE "${RESOURCE_FILES}"
  )
endif()

