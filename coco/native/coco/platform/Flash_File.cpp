#include "Flash_File.hpp"
#include <coco/assert.hpp>


namespace coco {

Flash_File::Flash_File(String name, int size, int pageSize, int blockSize)
	: file(fs::path(std::string(name.data(), name.size())), File::Mode::READ_WRITE)
	, size(size), pageSize(pageSize), blockSize(blockSize)
{
	// assert that sizes are power of 2
	assert((pageSize & (pageSize - 1)) == 0);
	assert((blockSize & (blockSize - 1)) == 0);

	// check that size consists of full pages
	assert((size & (pageSize - 1)) == 0);
}

// Buffer

Flash_File::Buffer::Buffer(int size, Flash_File &file)
	: BufferImpl(8, new uint8_t[size + 8] + 8, size, Buffer::State::READY), file(file)
{
}

Flash_File::Buffer::~Buffer() {
	delete [] (this->p.data - 8);
}

bool Flash_File::Buffer::start(Op op) {
	// check if READ, WRITE or ERASE flag is set
	assert((op & (Op::READ_WRITE | Op::ERASE)) != 0);

	int headerSize = this->p.headerSize;
	auto data = this->p.data;
	auto size = this->p.size;

	// get address and check alignment
	if (headerSize != 4) {
		// unsupported header size
		assert(false);
		return false;
	}
	uint32_t address = *(int32_t *)(data - 4);
	assert((address & (this->file.blockSize - 1)) == 0);

	auto &file = this->file.file;
	if ((op & Op::ERASE) == 0) {
		// check range
		assert(address + size <= this->file.size);
		if ((op & Op::WRITE) == 0) {
			// read
			file.read(address, data, size);
		} else {
			// write
			file.write(address, data, size);
		}
	} else {
		// erase page

		// align address to page
		int pageSize = this->file.pageSize;
		uint32_t a = address & (pageSize - 1);
		assert(a < this->file.size);

		// erase
		const uint8_t erased[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
		for (int i = 0; i < pageSize; i += 16) {
			file.write(a + i, erased, 16);
		}
	}

	setReady();
	return true;
}

bool Flash_File::Buffer::cancel() {
	return true;
}

} // namespace coco
