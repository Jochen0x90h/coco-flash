#pragma once

#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/Flash_flash.hpp>
#include <coco/board/config.hpp>


using namespace coco;

// address of last page in flash memory
const uint32_t FLASH_TEST_ADDRESS = FLASH_ADDRESS + FLASH_SIZE - flash::PAGE_SIZE;

// drivers for FlashTest
struct Drivers {
    Loop_TIM2 loop{SYS_CLOCK};
    Flash_flash flash;
    Flash_flash::Buffer<256> buffer{flash};
};

Drivers drivers;
