# CMakeList.txt: plik projektu CMake najwyższego poziomu, wykonaj konfigurację globalną
# i uwzględnij podprojekty w tym miejscu.
#
cmake_minimum_required (VERSION 3.8)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
project ("System-wymiany-plik-w-P2P")

# Uwzględnij podprojekty.
add_subdirectory ("Bencode")
add_subdirectory ("Client")

if(${BUILD_TRACKER})
	add_subdirectory ("Tracker")
	
endif()
