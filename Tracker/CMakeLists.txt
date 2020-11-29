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
)

add_executable (Tracker ${SOURCES})

target_include_directories(Tracker
          PUBLIC "${PROJECT_SOURCE_DIR}/Bencode"
)

target_link_libraries(Tracker PRIVATE Threads::Threads)

install(TARGETS Tracker DESTINATION bin)
