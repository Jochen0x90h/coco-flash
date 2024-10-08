add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
	PRIVATE
		dummy.cpp
)

if(${PLATFORM} STREQUAL "native" OR ${PLATFORM} STREQUAL "emu")
	# native platform (Windows, MacOS, Linux)
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/native FILES
			native/coco/platform/Flash_File.hpp
		PRIVATE
			native/coco/platform/Flash_File.cpp
	)
elseif(${PLATFORM} MATCHES "^nrf52")
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/nrf52 FILES
			nrf52/coco/platform/Flash_NVMC.hpp
		PRIVATE
			nrf52/coco/platform/Flash_NVMC.cpp
	)
elseif(${PLATFORM} MATCHES "^stm32f4")
	# not supported yet
elseif(${PLATFORM} MATCHES "^stm32")
	target_sources(${PROJECT_NAME}
		PUBLIC FILE_SET platform_headers TYPE HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/stm32 FILES
			stm32/coco/platform/Flash_FLASH.hpp
		PRIVATE
			stm32/coco/platform/Flash_FLASH.cpp
	)
endif()

target_link_libraries(${PROJECT_NAME}
	coco::coco
	coco-device::coco-device
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		..
)

# install the library
install(TARGETS ${PROJECT_NAME}
	FILE_SET headers DESTINATION include/coco
	FILE_SET platform_headers DESTINATION include
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
