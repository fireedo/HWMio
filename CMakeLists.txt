cmake_minimum_required(VERSION 3.5)

project(cpu_monitor)

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5Widgets REQUIRED)

set(SOURCES
    main.cpp
    SystemInfoWidget.cpp
    CpuTemperatureWidget.cpp
    GpuInfoWidget.cpp
    GpuOverclockWidget.cpp
)

set(HEADERS
    SystemInfoWidget.h
    CpuTemperatureWidget.h
    GpuInfoWidget.h
    GpuOverclockWidget.h
)

qt5_wrap_cpp(MOC_FILES
    SystemInfoWidget.h
    CpuTemperatureWidget.h
    GpuInfoWidget.h
    GpuOverclockWidget.h
)

add_executable(cpu_monitor ${SOURCES} ${MOC_FILES})
target_link_libraries(cpu_monitor Qt5::Widgets)
