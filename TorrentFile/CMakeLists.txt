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
# Dodaj źródło do pliku wykonywalnego tego projektu.

set(SOURCES 
	"File.h" 
	"File.cpp"
	"FileFrag.h"
	"FileFrag.cpp" 
    "InfoDictHash.cpp"
    "InfoDictHash.h"
	"Peer.h"
	"TorrentFile.h"
	"TorrentFile.cpp"
	"torrentReader.h"
	"torrentReader.cpp"
	"TrackerRequest.h"
	"TrackerRequest.cpp"
)

add_library (TorrentFile ${SOURCES})

target_include_directories(TorrentFile
          PUBLIC "${PROJECT_SOURCE_DIR}/Bencode"
)

target_link_libraries(TorrentFile Qt5::Widgets)
target_link_libraries(TorrentFile Bencode)
