file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/**.h"
)

add_executable(nwlib_tests ${SOURCES})

target_include_directories(nwlib_tests PUBLIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(nwlib_tests PRIVATE
	nw::lib)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	target_compile_options(nwlib_tests PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
		-Wno-gnu-anonymous-struct
		-Wno-c99-designator
	)
elseif (MSVC)
	target_compile_options(nwlib_tests PRIVATE
		/W4
		/WX
	)
endif ()

if (CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_CONFIGURATION_TYPES
		Debug
		Release
		Dist
		CACHE STRING "" FORCE
	)

	target_compile_definitions(nwlib_tests PRIVATE
		$<$<CONFIG:Debug>:NWLIB_DEBUG>
		$<$<CONFIG:Release>:NWLIB_RELEASE>
		$<$<CONFIG:Dist>:NWLIB_DIST>
	)
endif ()
