set(telekinesis_SRCS
  main.cpp
  LibraryItem.cpp
  LibraryModel.cpp
  MainWindow.cpp
)

add_executable(telekinesis ${telekinesis_SRCS})

target_link_libraries(
  telekinesis
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
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
