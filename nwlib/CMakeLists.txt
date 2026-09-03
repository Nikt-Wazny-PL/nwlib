file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/nwlib/**.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/nwlib/**.h"
)

add_library(nwlib STATIC ${SOURCES})
add_library(nw::lib ALIAS nwlib)

target_include_directories(nwlib PUBLIC 
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	target_compile_options(nwlib PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
		-Wno-gnu-anonymous-struct
		-Wno-c99-designator
	)
elseif (MSVC)
	target_compile_options(nwlib PRIVATE
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

	target_compile_definitions(nwlib PRIVATE
		$<$<CONFIG:Debug>:NWLIB_DEBUG>
		$<$<CONFIG:Release>:NWLIB_RELEASE>
		$<$<CONFIG:Dist>:NWLIB_DIST>
	)
endif ()
