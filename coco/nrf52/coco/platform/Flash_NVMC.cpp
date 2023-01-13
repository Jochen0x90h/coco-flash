#include "Flash_NVMC.hpp"
#include <coco/platform/platform.hpp>
#include <coco/assert.hpp>


namespace coco {

Flash_NVMC::Flash_NVMC(uint32_t baseAddress, int sectorCount, int sectorSize)
	: baseAddress(baseAddress), sectorCount(sectorCount), sectorSize(sectorSize)
{
	assert((baseAddress & (PAGE_SIZE - 1)) == 0);
	assert((sectorSize & (PAGE_SIZE - 1)) == 0);
}

Flash::Info Flash_NVMC::getInfo() {
	return {this->sectorCount, this->sectorSize, BLOCK_SIZE};
}

void Flash_NVMC::eraseSectorBlocking(int sectorIndex) {
	uint32_t address = this->baseAddress + sectorIndex * this->sectorSize;

	// set flash erase mode
	NRF_NVMC->CONFIG = N(NVMC_CONFIG_WEN, Een);

	// erase pages
	for (int i = 0; i < this->sectorSize; i += PAGE_SIZE) {
		NRF_NVMC->ERASEPAGE = address + i;

		// wait until flash is ready
		while (!NRF_NVMC->READY) {}
	}

	// set flash read mode
	NRF_NVMC->CONFIG = N(NVMC_CONFIG_WEN, Ren);
}

void Flash_NVMC::readBlocking(int address, void *data, int size) {
	auto dst = (uint32_t *)data;
	auto src = (const uint32_t *)(this->baseAddress + address);
	while (size >= 4) {
		// read 4 bytes
		*dst = *src;
		++dst;
		++src;
		size -= 4;
	}
	if (size > 0) {
		auto d = (uint8_t *)dst;
		auto s = (const uint8_t *)src;

		do {
			// read 1 byte
			*d = *s;
			++d;
			++s;
			--size;
		} while (size > 0);
	}
}

void Flash_NVMC::writeBlocking(int address, const void *data, int size) {
	// set flash write mode
	NRF_NVMC->CONFIG = N(NVMC_CONFIG_WEN, Wen);

	auto dst = (volatile uint32_t *)(this->baseAddress + address);
	auto src = (const uint32_t *)data;
	while (size >= 4) {
		// write 4 bytes
		*dst = *src;

		// data memory barrier
		__DMB();

		++dst;
		++src;
		size -= 4;

		// wait until flash is ready
		while (!NRF_NVMC->READY) {}
	}
	if (size > 0) {
		auto s = (const uint8_t *)src + size;
		uint32_t value = 0xffffffff;

		do {
			value <<= 8;

			// read 1 byte
			--s;
			value |= *s;
			--size;
		} while (size > 0);

		// write last 1-3 bytes
		*dst = value;

		// data memory barrier
		__DMB();

		// wait until flash is ready
		while (!NRF_NVMC->READY) {}
	}

	// set flash read mode
	NRF_NVMC->CONFIG = N(NVMC_CONFIG_WEN, Ren);
}

} // namespace coco
