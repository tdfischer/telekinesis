add_executable(telekinesis main.cpp MainWindow.cpp)

target_link_libraries(
  telekinesis
    Qt5::Core
    Qt5::Widgets
    ${GUPNP_AV_LIBRARIES}
    ${GUPNP_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    ${GSSDP_LIBRARIES}
)

set_target_properties(
  telekinesis
    PROPERTIES 
      AUTOUIC ON
      AUTOMOC ON
      CXX_STANDARD 11
)

include_directories(
  ${GUPNP_AV_INCLUDE_DIRS}
  ${GUPNP_INCLUDE_DIRS}
  ${GOBJECT_INCLUDE_DIRS}
  ${GSSDB_INCLUDE_DIRS}
)
