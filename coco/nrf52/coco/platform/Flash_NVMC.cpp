#include "Flash_NVMC.hpp"
#include <coco/platform/platform.hpp>
#include <coco/assert.hpp>


namespace coco {

bool Flash_NVMC::BufferBase::start(Op op) {
	// check if READ, WRITE or ERASE flag is set
	assert((op & (Op::READ_WRITE | Op::ERASE)) != 0);

	int headerSize = this->p.headerSize;
	auto data = this->p.data;
	int size = this->p.size;

	// get address and check alignment
	if (headerSize != 4) {
		// unsupported header size
		assert(false);
		return false;
	}
	uint32_t address = *(int32_t *)(data - 4);
	assert((address & (BLOCK_SIZE - 1)) == 0);

	if ((op & (Op::WRITE | Op::ERASE)) == 0) {
		// read
		auto src = (const uint32_t *)address;
		auto end = src + ((size + 3) >> 2);
		auto dst = (uint32_t *)data;
		while (src < end) {
			// read 4 bytes
			*dst = *src;
			++src;
			++dst;
		}
	} else if ((op & Op::ERASE) == 0) {
		// write
		// set flash write mode
		NRF_NVMC->CONFIG = N(NVMC_CONFIG_WEN, Wen);

		auto src = (const uint32_t *)data;
		auto end = src + ((size + 3) >> 2);
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

bool Flash_NVMC::BufferBase::cancel() {
	return true;
}

} // namespace coco
