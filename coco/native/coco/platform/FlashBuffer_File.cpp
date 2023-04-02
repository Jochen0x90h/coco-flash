#include "Flash_File.hpp"
#include <coco/assert.hpp>


namespace coco {

FlashBuffer_File::FlashBuffer_File(int capacity, const fs::path &filename, int pageSize, int blockSize)
	: Buffer(new uint8_t[capacity], capacity, Buffer::State::READY), file(filename, File::Mode::READ_WRITE), pageSize(pageSize), blockSize(blockSize)
{
	// assert that sizes are power of 2
	assert((pageSize & (pageSize - 1)) == 0);
	assert((blockSize & (blockSize - 1)) == 0);
}

FlashBuffer_File::~FlashBuffer_File() {
	delete [] this->buffer;
}

bool FlashBuffer_File::start(Op op) {
	if ((op & (Op::READ_WRITE | Op::ERASE)) == 0 || headerSize() != 4) {
		assert(false);
		return false;
	}

	// get address and check alignment
	uint32_t address = *reinterpret_cast<uint32_t *>(this->buffer);
	if ((address & (this->blockSize - 1)) != 0) {
		assert(false);
		return false;
	}
	auto data = this->p.data;
	auto size = this->p.size;

	if ((op & Op::READ) != 0) {
		// read
		this->file.read(address, data, size);
	} else if ((op & Op::WRITE) != 0) {
		// write
		this->file.write(address, data, size);
	} else {
		// erase page
		// align address to page
		uint32_t a = address & (this->pageSize - 1);
		const uint8_t erased[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
		for (int i = 0; i < this->pageSize; i += 16) {
			this->file.write(a + i, erased, 16);
		}
	}

	setReady();
	return true;
}

void FlashBuffer_File::cancel() {
}

} // namespace coco
