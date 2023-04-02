#pragma once

#include <coco/platform/Loop_native.hpp>
#include <coco/platform/Flash_File.hpp>


using namespace coco;

constexpr int STORAGE_ADDRESS = 0;

// drivers for FlashTest
struct Drivers {
	Loop_native loop;
	FlashBuffer_File buffer{4 + 256, "flashTest.bin", 4096, 4};
};
