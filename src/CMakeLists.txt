add_executable(telekinesis main.cpp MainWindow.cpp)

target_link_libraries(
  telekinesis
    Qt5::Core
    Qt5::Widgets
)

set_target_properties(
  telekinesis
    PROPERTIES 
      AUTOUIC ON
      AUTOMOC ON
)
