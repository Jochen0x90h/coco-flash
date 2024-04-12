#include "Flash_FLASH.hpp"
#include <coco/platform/platform.hpp>
#include <coco/platform/nvic.hpp>
#include <coco/assert.hpp>
#include <coco/debug.hpp>


#ifdef FLASH_SR_BSY1
#define FLASH_SR_BSY FLASH_SR_BSY1
#endif


namespace coco {

bool Flash_FLASH::BufferBase::start(Op op) {
	// check if READ, WRITE or ERASE flag is set
	assert((op & (Op::READ_WRITE | Op::ERASE)) != 0);

	// get header
	int headerSize = this->p.headerSize;
	if (headerSize != 4) {
		// unsupported header size
		assert(false);
		return false;
	}
	auto header = this->p.data;

	// get address and check alignment
	uint32_t address = *(int32_t *)header;
	assert((address & (BLOCK_SIZE - 1)) == 0);

	auto data = header + headerSize;
	int size = this->p.size - headerSize;

	if ((op & (Op::WRITE | Op::ERASE)) == 0) {
		// read
		auto src = (const Block *)address;
		auto end = src + uint32_t(size + sizeof(Block) - 1) / sizeof(Block);
		auto dst = (Block *)data;
		while (src < end) {
			// read block
			*dst = *src;
			++src;
			++dst;
		}
	} else {
		// write or erase
		// unlock flash
		FLASH->KEYR = 0x45670123;
		FLASH->KEYR = 0xCDEF89AB;

		// steps 1 and 2 in reference manual
		while ((FLASH->SR & FLASH_SR_BSY) != 0) {}
		FLASH->SR = 0xffff;

		if ((op & Op::ERASE) == 0) {
			// write

			// set flash write mode
			FLASH->CR = FLASH_CR_PG;

			auto src = (const Block *)data;
			auto end = src + uint32_t(size + sizeof(Block) - 1) / sizeof(Block);
			auto dst = (Block *)address;
			while (src < end) {
				// write block
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
		} else {
			// erase page
#ifdef FLASH_AR_FAR
			// erase using address

			// set flash page erase mode
			FLASH->CR = FLASH_CR_PER;

			// set address of page to erase (must come after PER)
			FLASH->AR = address;

			// start page erase
			FLASH->CR = FLASH_CR_PER | FLASH_CR_STRT;
#endif
#ifdef FLASH_CR_PNB
			// erase using page index
			int page = (address - 0x8000000u) / PAGE_SIZE;
			int PNB = (page << FLASH_CR_PNB_Pos);

			// set flash page erase mode
			FLASH->CR = FLASH_CR_PER | PNB;

			// start page erase
			FLASH->CR = FLASH_CR_PER | PNB | FLASH_CR_STRT;
#endif

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

		// lock flash (and clear PG or PER bit)
		FLASH->CR = FLASH_CR_LOCK;

		// flush caches
		// https://github.com/zephyrproject-rtos/zephyr/pull/32218/files#diff-20dbbd4b4adb0bda3e1b8f32c0e8db33501947942184928d542446d260a2eaae
#ifdef FLASH_ACR_DCEN
		// get ACR
		uint32_t ACR = FLASH->ACR;

		// disable caches
		uint32_t disabled = ACR & ~(FLASH_ACR_DCEN | FLASH_ACR_ICEN);
		FLASH->ACR = disabled;

		// reset caches while disabled
		FLASH->ACR = disabled | (FLASH_ACR_DCRST |  FLASH_ACR_ICRST);

		// restore ACR
		FLASH->ACR = ACR;
#endif
	}

	setReady(size);
	return true;
}

bool Flash_FLASH::BufferBase::cancel() {
	return true;
}

} // namespace coco
