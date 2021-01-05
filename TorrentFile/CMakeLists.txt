# CMakeList.txt: projekt CMake dla elementu Bencode, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Dodaj źródło do pliku wykonywalnego tego projektu.

set(SOURCES 
	"Peer.h"
)

add_library (TorrentFile ${SOURCES})
