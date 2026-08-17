add_library(sfc STATIC
    "audio.c"

    "audio_backend.h"
	"audio_wasapi.c"

	"wav.h"
	"wav.c"

	"sound.c"

   
    "../include/sfc/audio.h"
	"../include/sfc/sound.h"
)

add_library(sfc::sfc ALIAS sfc)

target_include_directories(sfc 
	PUBLIC
		$<BUILD_INTERFACE:${SOUNDFORC_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${SOUNDFORC_BINARY_DIR}/include>
		$<INSTALL_INTERFACE:include>

	PRIVATE
		$<BUILD_INTERFACE:${SOUNDFORC_SOURCE_DIR}/include/sfc>
)