add_executable (example 
	"main.c"
)

target_link_libraries(example
    PRIVATE
        caudio
)


# target_link_libraries(caudio
# 	PRIVATE
# 		ole32
# 		uuid
# )