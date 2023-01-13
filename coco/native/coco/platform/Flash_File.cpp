#include "Flash_File.hpp"
#include <coco/assert.hpp>


namespace coco {

Flash_File::Flash_File(const fs::path &filename, int sectorCount, int sectorSize, int blockSize)
	: file(filename, File::Mode::READ_WRITE), sectorCount(sectorCount), sectorSize(sectorSize), blockSize(blockSize)
{
	int size = sectorCount * sectorSize;

	// set size of emulated flash and initialize to 0xff if necessary
	this->file.resize(size, 0xff);
}

Flash::Info Flash_File::getInfo() {
	return {this->sectorCount, this->sectorSize, this->blockSize};
}

void Flash_File::eraseSectorBlocking(int sectorIndex) {
	// check range
	assert(sectorIndex >= 0 && sectorIndex < this->sectorCount);

	// erase sector
	this->file.fill(sectorIndex * this->sectorSize, 0xff, this->sectorSize);
}

void Flash_File::readBlocking(int address, void *data, int size) {
	// check block alignment
	assert(address % this->blockSize == 0);

	// check range
	assert(address >= 0 && address + size <= this->sectorCount * this->sectorSize);

	// read from file
	this->file.read(address, data, size);
}

void Flash_File::writeBlocking(int address, const void *data, int size) {
	// check block alignment
	assert(address % this->blockSize == 0);

	// check range
	assert(address >= 0 && address + size <= this->sectorCount * this->sectorSize);

	// write to file
	this->file.write(address, data, size);
}

} // namespace coco
