#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/Flash_FLASH.hpp>
#include <coco/board/config.hpp>


using namespace coco;

constexpr uint32_t FLASH_TEST_ADDRESS = FLASH_ADDRESS + FLASH_SIZE - Flash_FLASH::PAGE_SIZE;

// drivers for FlashTest
struct Drivers {
	Loop_TIM2 loop;
	Flash_FLASH::Buffer<256> buffer;
};
