# CMakeList.txt: projekt CMake dla elementu Bencode, dołącz źródło i zdefiniuj
# tutaj logikę specyficzną dla projektu.
#
cmake_minimum_required (VERSION 3.8)

# Dodaj źródło do pliku wykonywalnego tego projektu.

set(TYPES_SOURCES 
	"Types/ICode.h" 
	"Types/String.cpp"
	"Types/String.h"
	"Types/Dictionary.cpp" 
	"Types/Dictionary.h"
	"Types/Int.cpp"
	"Types/Int.h"
	"Types/List.cpp"
	"Types/List.h"
)

set(CODERS_SOURCES
	"Coders/Encoder.cpp"
	"Coders/Encoder.h"
	"Coders/Decoder.cpp"
	"Coders/Decoder.h"
)

add_library (Bencode "Bencode.h" "Coders.h" "Types.h" ${TYPES_SOURCES} ${CODERS_SOURCES})
