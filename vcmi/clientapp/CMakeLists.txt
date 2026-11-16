set(clientapp_SRCS
		StdInc.cpp
		EntryPoint.cpp
)

set(clientapp_HEADERS
		StdInc.h
		EntryPoint.h
)

assign_source_group(${clientapp_SRCS} ${clientapp_HEADERS})

# The client is built as a static library that provides vcmiclientMain().
# The actual executable that provides main() lives in src/client.
add_library(vcmiclientstatic STATIC ${clientapp_SRCS} ${clientapp_HEADERS})

target_link_libraries(vcmiclientstatic PRIVATE vcmiclientcommon)

target_include_directories(vcmiclientstatic
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

vcmi_set_output_dir(vcmiclientstatic "")
enable_pch(vcmiclientstatic)

#install icons and desktop file on Linux
if(NOT WIN32 AND NOT APPLE AND NOT ANDROID)
	foreach(iconSize 16 22 32 48 64 128 256 512 1024 2048)
		install(FILES "icons/vcmiclient.${iconSize}x${iconSize}.png"
			DESTINATION "share/icons/hicolor/${iconSize}x${iconSize}/apps"
			RENAME vcmiclient.png
		)
	endforeach()

	install(FILES icons/vcmiclient.svg
		DESTINATION share/icons/hicolor/scalable/apps
		RENAME vcmiclient.svg
	)
	install(FILES icons/vcmiclient.desktop
		DESTINATION share/applications
	)
endif()
