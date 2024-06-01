cmake_minimum_required(VERSION 3.14)

project(HWMio VERSION 0.1.0)

# Find either Qt6 or Qt5
find_package(Qt6 COMPONENTS Widgets REQUIRED)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include directories
if (Qt6_FOUND)
    include_directories(${Qt6Widgets_INCLUDE_DIRS})
    add_definitions(${Qt6Widgets_DEFINITIONS})
else()
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    add_definitions(${Qt5Widgets_DEFINITIONS})
endif()

set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    CpuTemperatureWidget.cpp
    CpuTemperatureWidget.h
    GpuInfoWidget.cpp
    GpuInfoWidget.h
    GpuOverclockWidget.cpp
    GpuOverclockWidget.h
    SystemInfoWidget.cpp
    SystemInfoWidget.h
)

if (${CMAKE_VERSION} VERSION_LESS "3.15")
    if (Qt6_FOUND)
        add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060000)
    else()
        add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050000)
    endif()
else()
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

add_executable(HWMio ${PROJECT_SOURCES})

# Link libraries
if (Qt6_FOUND)
    target_link_libraries(HWMio Qt6::Widgets)
else()
    target_link_libraries(HWMio Qt5::Widgets)
endif()

# Manually specify paths for installed packages
include_directories(/usr/include)
link_directories(/usr/lib64)

# Ensure the install_policy.sh script is executable
configure_file(install_policy.sh install_policy.sh COPYONLY)
install(PROGRAMS ${CMAKE_BINARY_DIR}/install_policy.sh DESTINATION ${CMAKE_BINARY_DIR})

# Install Polkit policy script
install(CODE "execute_process(COMMAND chmod +x ${CMAKE_BINARY_DIR}/install_policy.sh)")
install(CODE "execute_process(COMMAND sh ${CMAKE_BINARY_DIR}/install_policy.sh)")
