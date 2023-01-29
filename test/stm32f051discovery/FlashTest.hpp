#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/Flash_FLASH.hpp>


// drivers for FlashTest
struct Drivers {
	// flash start address
	static constexpr int FLASH_ADDRESS = 0x8000000 + 32768;

	coco::Loop_TIM2 loop;
	coco::Flash_FLASH flash{FLASH_ADDRESS, 2, 4096};
};
