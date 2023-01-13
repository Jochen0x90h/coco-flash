#pragma once

#include <coco/Flash.hpp>


namespace coco {

/**
 * Implementation of flash interface for nrf52
 * https://infocenter.nordicsemi.com/index.jsp?topic=%2Fps_nrf52840%2Fmemory.html&cp=4_0_0_3_1_1&anchor=flash
 * https://infocenter.nordicsemi.com/index.jsp?topic=%2Fps_nrf52840%2Fnvmc.html&cp=4_0_0_3_2
 * 
 * Resources:
 *	NRF_NVMC
 */
class Flash_NVMC : public Flash {
public:
	// size of a page that has to be erased at once
	static constexpr int PAGE_SIZE = 4096;

	// size of a block that has to be written at once
	static constexpr int BLOCK_SIZE = 4;

	/**
	 * Constructor
	 * @param baseAddress base address of flash, must be a multiple of PAGE_SIZE
	 * @param sectorCount number of sectors
	 * @param sectorSize size of one sector, must be a multiple of PAGE_SIZE
	 */
	Flash_NVMC(uint32_t baseAddress, int sectorCount, int sectorSize);
	Info getInfo() override;
	void eraseSectorBlocking(int sectorIndex) override;
	void readBlocking(int address, void *data, int size) override;
	void writeBlocking(int address, const void *data, int size) override;
	using Flash::readBlocking;
	using Flash::writeBlocking;

protected:
	uint32_t baseAddress;
	int sectorCount;
	int sectorSize;
};

} // namespace coco
