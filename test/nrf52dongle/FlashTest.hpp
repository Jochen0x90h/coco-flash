#pragma once

#include <coco/platform/Loop_RTC0.hpp>
#include <coco/platform/Flash_NVMC.hpp>


// drivers for FlashTest
struct Drivers {
	coco::Loop_RTC0 loop;
	coco::Flash_NVMC flash{0xe0000 - 2 * 4096, 2, 4096};
};
