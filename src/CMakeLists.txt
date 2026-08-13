add_library(audio STATIC
    "audio.c"
	"wav.c"
   

    "../include/audio/audio.h"
	"../include/audio/wav.h"
)

add_library(audio::audio ALIAS audio)

target_include_directories(audio 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
		$<INSTALL_INTERFACE:include>

	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/cecs>
)