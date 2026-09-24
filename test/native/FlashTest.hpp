#pragma once

#include <coco/platform/Loop_native.hpp>
#include <coco/platform/Flash_File.hpp>


using namespace coco;

constexpr uint32_t FLASH_ADDRESS = 0;
constexpr uint32_t FLASH_SIZE = 4096;

// address and size of emulated flash page 1 for testing
constexpr uint32_t FLASH_TEST_ADDRESS = 2048;
constexpr uint32_t FLASH_TEST_SIZE = 4096;

// drivers for FlashTest
struct Drivers {
	Loop_native loop;

	// emulated flash
	Flash_File flash{"flashTest.bin", 4096, 2048, 4};
	Flash_File::Buffer buffer{256, flash};
};

Drivers drivers;
