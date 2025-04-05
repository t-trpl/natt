cmake_minimum_required(VERSION 3.10)
project(natt)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(APPINDICATOR REQUIRED ayatana-appindicator3-0.1)
pkg_check_modules(X11 REQUIRED x11)
pkg_check_modules(XRANDR REQUIRED xrandr)

add_executable(${PROJECT_NAME} src/main.c)

aux_source_directory(src SRC)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GTK3_INCLUDE_DIRS} 
    ${APPINDICATOR_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${XRANDR_INCLUDE_DIRS}

)
target_sources(${PROJECT_NAME} PRIVATE
    ${SRC}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${GTK3_LIBRARIES} 
    ${APPINDICATOR_LIBRARIES}
    ${X11_LIBRARIES}
    ${XRANDR_LIBRARIES}
    m
)

target_compile_options(${PROJECT_NAME} PRIVATE ${GTK3_CFLAGS_OTHER} -O2 -Wall)
