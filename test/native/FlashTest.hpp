#pragma once

#include <coco/platform/Loop_native.hpp>
#include <coco/platform/Flash_File.hpp>


// drivers for FlashTest
struct Drivers {
	coco::Loop_native loop;
	coco::Flash_File flash{"flashTest.bin", 2, 4096, 4};
};
