#include "Flash_FLASH.hpp"
#include <coco/platform/platform.hpp>
#include <coco/assert.hpp>


namespace coco {

bool Flash_FLASH::BufferBase::setHeader(const uint8_t *data, int size) {
	if (size != 4) {
		assert(false);
		return false;
	}
	this->address = *reinterpret_cast<const uint32_t *>(data);
	return true;
}

bool Flash_FLASH::BufferBase::startInternal(int size, Op op) {
	// check if READ, WRITE or ERASE flag is set
	assert((op & (Op::READ_WRITE | Op::ERASE)) != 0);

	// get address and check alignment
	uint32_t address = this->address;
	assert((address & (BLOCK_SIZE - 1)) == 0);
	auto data = this->dat;

	if ((op & (Op::WRITE | Op::ERASE)) == 0) {
		// read
		auto src = (const uint16_t *)address;
		auto end = src + ((size + 1) >> 1);
		auto dst = (uint16_t *)data;
		while (src < end) {
			// read 2 bytes
			*dst = *src;
			++src;
			++dst;
		}
	} else if ((op & Op::ERASE) == 0) {
		// write
		// unlock flash
		FLASH->KEYR = 0x45670123;
		FLASH->KEYR = 0xCDEF89AB;

		// set flash write mode
		FLASH->CR = FLASH_CR_PG;

		auto src = (const uint16_t *)data;
		auto end = src + ((size + 1) >> 1);
		auto dst = (uint16_t *)address;
		while (src < end) {
			// write 2 bytes
			*dst = *src;

			// data memory barrier
			__DMB();

			++src;
			++dst;

			// wait until flash is ready
			while ((FLASH->SR & FLASH_SR_BSY) != 0) {}

			// check result
			if ((FLASH->SR & FLASH_SR_EOP) != 0) {
				// ok
				FLASH->SR = FLASH_SR_EOP;
			} else {
				// error
			}
		}

		// lock flash (and clear PG bit)
		FLASH->CR = FLASH_CR_LOCK;
	} else {
		// erase page
		// unlock flash
		FLASH->KEYR = 0x45670123;
		FLASH->KEYR = 0xCDEF89AB;

		// set page erase mode
		FLASH->CR = FLASH_CR_PER;

		// set address of page to erase
		FLASH->AR = address;

		// start page erase
		FLASH->CR = FLASH_CR_PER | FLASH_CR_STRT;

		// wait until flash is ready
		while ((FLASH->SR & FLASH_SR_BSY) != 0) {}

		// check result
		if ((FLASH->SR & FLASH_SR_EOP) != 0) {
			// ok
			FLASH->SR = FLASH_SR_EOP;
		} else {
			// error
		}

		// lock flash (and clear PER bit)
		FLASH->CR = FLASH_CR_LOCK;
	}

	setReady(size);
	return true;
}

void Flash_FLASH::BufferBase::cancel() {
}

} // namespace coco
