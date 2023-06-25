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

Flash_File::Buffer::Buffer(Flash_File &file, int size)
	: BufferImpl(new uint8_t[size], size, Buffer::State::READY), file(file)
{
}

Flash_File::Buffer::~Buffer() {
	delete [] this->dat;
}

bool Flash_File::Buffer::setHeader(const uint8_t *data, int size) {
	if (size != 4) {
		assert(false);
		return false;
	}
	this->address = *reinterpret_cast<const uint32_t *>(data);
	return true;
}

bool Flash_File::Buffer::startInternal(int size, Op op) {
	// check if READ, WRITE or ERASE flag is set
	assert((op & (Op::READ_WRITE | Op::ERASE)) != 0);

	this->xferred = size;

	// get address
	auto address = this->address;
	auto data = this->dat;

	auto &file = this->file.file;
	if ((op & Op::ERASE) == 0) {
		// check alignment
		assert(uint32_t(address + size) <= this->file.size && (address & (this->file.blockSize - 1)) == 0);
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

void Flash_File::Buffer::cancel() {
}

} // namespace coco
