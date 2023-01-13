#pragma once

#include <coco/Flash.hpp>


namespace coco {

/**
 * Implementation of flash interface for stm32f0
 * Reference manual: https://www.st.com/resource/en/reference_manual/dm00031936-stm32f0x1stm32f0x2stm32f0x8-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 *
 * Resources:
 *	FLASH: Flash controller (reference manual section 3)
 */
class Flash_FLASH : public Flash {
public:
	// size of a page that has to be erased at once
	static constexpr int PAGE_SIZE = 1024;

	// size of a block that has to be written at once
	static constexpr int BLOCK_SIZE = 2;

	/**
	 * Constructor
	 * @param baseAddress base address of flash, must be a multiple of PAGE_SIZE
	 * @param sectorCount number of sectors
	 * @param sectorSize size of one sector, must be a multiple of PAGE_SIZE
	 */
	Flash_FLASH(uint32_t baseAddress, int sectorCount, int sectorSize);
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
