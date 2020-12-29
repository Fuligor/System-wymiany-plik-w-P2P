# CMakeList.txt: projekt CMake dla elementu Bencode, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
# Dodaj źródło do pliku wykonywalnego tego projektu.

set(SOURCES 
    "main.cpp" 
    "BitSet.cpp"
    "BitSet.h"
    "Client.cpp"
    "Client.h"
    "ShareFileWindow.cpp"
    "ShareFileWindow.h"
    "ShareFileWindow.ui"
    "PeerConnection.cpp"
    "PeerConnection.h"
    "TrackerConnection.cpp"
    "TrackerConnection.h"
    "Torrent.cpp"
    "Torrent.h"
    "TorrentDownloader.cpp"
    "TorrentDownloader.h"
    "TorrentDownloadStatus.h"
    "TorrentFileStatus.h"
    "TorrentManager.cpp"
    "TorrentManager.h"
    "Window.cpp" 
    "Window.h"
    "Window.ui"
    "Window.qrc"
)

add_executable (Client ${SOURCES})

target_include_directories(Client
          PUBLIC "${PROJECT_SOURCE_DIR}/Bencode"
)
target_include_directories(Client
          PUBLIC "${PROJECT_SOURCE_DIR}/TorrentFile"
)

install(TARGETS Client DESTINATION bin)

target_link_libraries(Client Bencode)
target_link_libraries(Client TorrentFile)
target_link_libraries(Client Qt5::Widgets)
target_link_libraries(Client Qt5::Network)
