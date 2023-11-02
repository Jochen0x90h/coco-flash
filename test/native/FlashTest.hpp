#pragma once

#include <coco/platform/Loop_native.hpp>
#include <coco/platform/Flash_File.hpp>


using namespace coco;

constexpr uint32_t FLASH_TEST_ADDRESS = 0;

// drivers for FlashTest
struct Drivers {
	Loop_native loop;
	Flash_File file{"flashTest.bin", 4096, 4096, 4};
	Flash_File::Buffer buffer{256, file};
};

Drivers drivers;
