add_library(caudio STATIC
    "audio.c"
	"wav.c"
   
    "../include/caudio/audio.h"
	"../include/caudio/wav.h"
)

add_library(caudio::caudio ALIAS caudio)

target_include_directories(caudio 
	PUBLIC
		$<BUILD_INTERFACE:${CAUDIO_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CAUDIO_BINARY_DIR}/include>
		$<INSTALL_INTERFACE:include>

	PRIVATE
		$<BUILD_INTERFACE:${CAUDIO_SOURCE_DIR}/include/caudio>
)