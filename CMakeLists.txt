                                                                                                                                                                                                                                                                                                            cmake_minimum_required(VERSION 3.16)

project(InformationBoard VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network Sql Multimedia)

# apikeys.h содержит личный API-ключ и не хранится в репозитории (см. .gitignore).
# При первой сборке из шаблона создаётся пустой файл, чтобы проект собирался
# "из коробки" — ключ погоды можно будет вписать позже через диалог "Настройки".
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/src/apikeys.h")
    configure_file(
        "${CMAKE_SOURCE_DIR}/src/apikeys.h.example"
        "${CMAKE_SOURCE_DIR}/src/apikeys.h"
        COPYONLY
    )
endif()

add_executable(InformationBoard
    src/main.cpp
    src/mainwindow.cpp
    src/Board/boardwindow.cpp
    src/Board/draggableelement.cpp
    src/Board/boarddesignerwidget.cpp
    src/Board/elementpropertiesdialog.cpp
    src/Weather/weathermanager.cpp
    src/News/newsmanager.cpp
    src/Database/databaseemanager.cpp
    src/Database/databasewidget.cpp
    src/Schedule/schedulemanager.cpp
    src/Schedule/scheduleentrydialog.cpp
    src/Schedule/schedulewidget.cpp
    src/Settings/settingsdialog.cpp
    src/Announcements/announcementmanager.cpp
    src/Announcements/announcemententrydialog.cpp
    src/Announcements/announcementwidget.cpp
    src/Media/slideshowmanager.cpp
    src/mainwindow.h
    src/Board/boardwindow.h
    src/Board/draggableelement.h
    src/Board/boarddesignerwidget.h
    src/Board/elementpropertiesdialog.h
    src/Weather/weathermanager.h
    src/News/newsmanager.h
    src/Database/databaseemanager.h
    src/Database/databasewidget.h
    src/Schedule/schedulemanager.h
    src/Schedule/scheduleentrydialog.h
    src/Schedule/schedulewidget.h
    src/Settings/settingsdialog.h
    src/Announcements/announcementmanager.h
    src/Announcements/announcemententrydialog.h
    src/Announcements/announcementwidget.h
    src/Media/slideshowmanager.h
    src/apikeys.h
    src/mainwindow.ui
)

target_link_libraries(InformationBoard PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Sql
    Qt5::Multimedia
)