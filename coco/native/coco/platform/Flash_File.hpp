#pragma once

#include <coco/Flash.hpp>
#include <coco/platform/File.hpp>
#include <string>


namespace coco {

/**
 * Implementation of a file based flash emulation
 */
class Flash_File : public Flash {
public:
	/**
	 * Constructor
	 * @param filename name of file for the flash emulation
	 * @param sectorCount number of sectors
	 * @param sectorSize size of one sector
	 * @param blockSize size of block that has to be written at once
	 */
	Flash_File(const fs::path &filename, int sectorCount, int sectorSize, int blockSize);

	Info getInfo() override;
	void eraseSectorBlocking(int sectorIndex) override;
	void readBlocking(int address, void *data, int size) override;
	void writeBlocking(int address, const void *data, int size) override;
	using Flash::readBlocking;
	using Flash::writeBlocking;

protected:
	File file;
	int sectorCount;
	int sectorSize;
	int blockSize;
};

} // namespace coco
