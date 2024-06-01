cmake_minimum_required(VERSION 3.14)
project(cpu_monitor)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5Widgets REQUIRED)

set(SOURCE_FILES
    main.cpp
    MainWindow.cpp
    SystemInfoWidget.cpp
    CpuTemperatureWidget.cpp
    GpuInfoWidget.cpp
    GpuOverclockWidget.cpp
)

set(HEADER_FILES
    MainWindow.h
    SystemInfoWidget.h
    CpuTemperatureWidget.h
    GpuInfoWidget.h
    GpuOverclockWidget.h
)

qt5_wrap_cpp(MOC_FILES ${HEADER_FILES})

add_executable(cpu_monitor
    ${SOURCE_FILES}
    ${MOC_FILES}
)

target_link_libraries(cpu_monitor Qt5::Widgets)
