# CMakeList.txt: projekt CMake dla elementu Bencode, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#
cmake_minimum_required (VERSION 3.8)

# Dodaj źródło do pliku wykonywalnego tego projektu.

set(SOURCES 
	"File.h" 
	"File.cpp"
	"FileFrag.h"
	"FileFrag.cpp" 
	"TorrentFile.h"
	"TorrentFile.cpp"
)

add_library (TorrentFile ${SOURCES})