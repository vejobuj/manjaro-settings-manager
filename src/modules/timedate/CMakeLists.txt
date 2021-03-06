include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MSM_TIMEDATE_SRCS
    LocaleGlobal.cpp
    PageTimeDate.cpp
    TimeDate.cpp
    TimeZoneDialog.cpp
    TimeZoneWidget.cpp)

set(MSM_TIMEDATE_UI
    ui/PageTimeDate.ui
    ui/TimeZoneDialog.ui)
ki18n_wrap_ui(MSM_TIMEDATE_SRCS ${MSM_TIMEDATE_UI})

set(MSM_TIMEDATE_RESOURCES
    timedate.qrc)
QT5_ADD_RESOURCES (MSM_TIMEDATE_SRCS ${MSM_TIMEDATE_RESOURCES})

add_library(msm_timedate MODULE ${MSM_TIMEDATE_SRCS})
target_compile_definitions(msm_timedate PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")
target_link_libraries(msm_timedate
    Qt5::DBus
    Qt5::Widgets
    KF5::ConfigWidgets
    KF5::CoreAddons
    KF5::I18n)

install(TARGETS msm_timedate DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES msm_timedate.desktop DESTINATION ${SERVICES_INSTALL_DIR})
