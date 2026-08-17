add_executable (example 
	"main.c"
)

target_link_libraries(example
    PRIVATE
        sfc
)


# target_link_libraries(sfc
# 	PRIVATE
# 		ole32
# 		uuid
# )