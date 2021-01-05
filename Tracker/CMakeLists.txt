# CMakeList.txt: projekt CMake dla elementu Bencode, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

# Dodaj źródło do pliku wykonywalnego tego projektu.

set(SOURCES 
	"main.cpp"
    "Server.cpp"
    "Server.h"
    "Connection.cpp"
    "Connection.h"
    "TorrentInstanceInfo.cpp"
    "TorrentInstanceInfo.h"
    "TrackerResponse.cpp"
    "TrackerResponse.h"
)

add_executable (Tracker ${SOURCES})

target_include_directories(Tracker
          PUBLIC "${PROJECT_SOURCE_DIR}/Bencode"
)
target_include_directories(Tracker
          PUBLIC "${PROJECT_SOURCE_DIR}/TorrentFile"
)
target_link_libraries(Tracker PRIVATE Threads::Threads)
target_link_libraries(Tracker PRIVATE Bencode)