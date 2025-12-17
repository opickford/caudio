add_executable (example 
	"main.c"
)

target_link_libraries(example
    PRIVATE
        audio
)


# target_link_libraries(audio
# 	PRIVATE
# 		ole32
# 		uuid
# )