cmake_minimum_required(VERSION 3.10)
project(natt)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(APPINDICATOR REQUIRED ayatana-appindicator3-0.1)
pkg_check_modules(X11 REQUIRED x11)
pkg_check_modules(XRANDR REQUIRED xrandr)

add_executable(natt 
    src/main.c 
    src/xsctcf.c 
    src/io.c 
    src/gtk_creation.c 
    src/util.c 
    src/daemon.c
)

target_include_directories(natt PRIVATE 
    ${GTK3_INCLUDE_DIRS} 
    ${APPINDICATOR_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${XRANDR_INCLUDE_DIRS}
)

target_link_libraries(natt PRIVATE 
    ${GTK3_LIBRARIES} 
    ${APPINDICATOR_LIBRARIES}
    ${X11_LIBRARIES}
    ${XRANDR_LIBRARIES}
    m
)

target_compile_options(natt PRIVATE ${GTK3_CFLAGS_OTHER} -O2 -Wall)
