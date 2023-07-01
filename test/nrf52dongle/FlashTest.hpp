#pragma once

#include <coco/platform/Loop_RTC0.hpp>
#include <coco/platform/Flash_NVMC.hpp>
#include <coco/board/config.hpp>


using namespace coco;

constexpr uint32_t FLASH_TEST_ADDRESS = FLASH_ADDRESS + 0xe0000 - Flash_NVMC::PAGE_SIZE;

// drivers for FlashTest
struct Drivers {
	Loop_RTC0 loop;
	Flash_NVMC::Buffer<256> buffer;
};
