#include "Flash_NVMC.hpp"
#include <coco/platform/platform.hpp>
#include <coco/assert.hpp>


namespace coco {

bool Flash_NVMC::BufferBase::setHeader(const uint8_t *data, int size) {
	if (size != 4) {
		assert(false);
		return false;
	}
	this->address = *reinterpret_cast<const uint32_t *>(data);
	return true;
}

bool Flash_NVMC::BufferBase::startInternal(int size, Op op) {
	// check if READ, WRITE or ERASE flag is set
	assert((op & (Op::READ_WRITE | Op::ERASE)) != 0);

	// get address and check alignment
	uint32_t address = this->address;
	assert((address & (BLOCK_SIZE - 1)) == 0);
	auto data = this->dat;

	if ((op & (Op::WRITE | Op::ERASE)) == 0) {
		// read
		auto src = (const uint32_t *)address;
		auto end = src + (size >> 2);
		auto dst = (uint32_t *)data;
		while (src < end) {
			// read 4 bytes
			*dst = *src;
			++src;
			++dst;
		}
		int tail = size & 3;
		if (tail > 0) {
			auto d = (uint8_t *)dst;
			auto s = (const uint8_t *)src;
			do {
				// read 1 byte
				*d = *s;
				++s;
				++d;
				--tail;
			} while (tail > 0);
		}
	} else if ((op & Op::ERASE) == 0) {
		// write
		// set flash write mode
		NRF_NVMC->CONFIG = N(NVMC_CONFIG_WEN, Wen);

		auto src = (const uint32_t *)data;
		auto end = src + (size >> 2);
		auto dst = (uint32_t *)address;
		while (src < end) {
			// write 4 bytes
			*dst = *src;

			// data memory barrier
			__DMB();

			++src;
			++dst;

			// wait until flash is ready
			while (!NRF_NVMC->READY) {}
		}
		int tail = size & 3;
		if (tail > 0) {
			auto s = (const uint8_t *)src + size;
			uint32_t value = 0xffffffff;

			do {
				value <<= 8;

				// read 1 byte
				--s;
				value |= *s;
				--tail;
			} while (tail > 0);

			// write last 1-3 bytes
			*dst = value;

			// data memory barrier
			__DMB();

			// wait until flash is ready
			while (!NRF_NVMC->READY) {}
		}

		// set flash read mode
		NRF_NVMC->CONFIG = N(NVMC_CONFIG_WEN, Ren);
	} else {
		// erase page
		// set flash erase mode
		NRF_NVMC->CONFIG = N(NVMC_CONFIG_WEN, Een);

		// erase page
		NRF_NVMC->ERASEPAGE = address;

		// wait until flash is ready
		while (!NRF_NVMC->READY) {}

		// set flash read mode
		NRF_NVMC->CONFIG = N(NVMC_CONFIG_WEN, Ren);
	}

	setReady(size);
	return true;
}

void Flash_NVMC::BufferBase::cancel() {
}

} // namespace coco
