cmake_minimum_required(VERSION 3.16)
project(GBFR_SkillEditor LANGUAGES CXX)

# ── C++ 标准 ──
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Qt 元对象编译 ──
set(CMAKE_AUTOMOC ON)

# ── 查找 Qt 库 ──
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ── 可执行文件（GUI 程序，不显示命令行窗口） ──
add_executable(${PROJECT_NAME} WIN32
    main.cpp
    MainWindow.cpp
    MainWindow.h
    RecordParser.h
    app_icon.rc          # Windows 可执行文件图标
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

# ── 资源文件 ──
# 复制 QSS 样式表和路径名称对照表到构建目录
configure_file(${CMAKE_SOURCE_DIR}/style.qss
               ${CMAKE_BINARY_DIR}/style.qss
               COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/skill_id_map.json
               ${CMAKE_BINARY_DIR}/skill_id_map.json
               COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/app_icon.ico
               ${CMAKE_BINARY_DIR}/app_icon.ico
               COPYONLY)

# ── 安装规则（用于打包分发） ──
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/dist)
install(FILES ${CMAKE_SOURCE_DIR}/style.qss
        DESTINATION ${CMAKE_BINARY_DIR}/dist)
install(FILES ${CMAKE_SOURCE_DIR}/skill_id_map.json
        DESTINATION ${CMAKE_BINARY_DIR}/dist)
