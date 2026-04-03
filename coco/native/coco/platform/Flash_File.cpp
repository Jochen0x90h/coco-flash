#include "Flash_File.hpp"
#include <cassert>


namespace coco {

static const uint8_t erased[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

Flash_File::Flash_File(String name, int size, int pageSize, int blockSize)
    : file_(fs::path(std::string(name.data(), name.size())), NativeFile::Mode::CREATE_OR_OPEN)
    , size_(size), pageSize_(pageSize), blockSize_(blockSize)
{
    // assert that sizes are power of 2
    assert((pageSize & (pageSize - 1)) == 0);
    assert((blockSize & (blockSize - 1)) == 0);

    // check that size consists of full pages
    assert((size & (pageSize - 1)) == 0);

    auto fileSize = file_.size();
    assert(fileSize >= 0);
    if (fileSize >= 0) {
        if (fileSize <= size) {
            // fill emulated flash file with 0xff
            int toFill = size - int(fileSize);

            int remaining = toFill & 15;
            if (remaining > 0)
                file_.write(fileSize, erased, remaining);

            int offset = int(fileSize) + remaining;
            int count = toFill >> 4;
            for (int i = 0; i < count; ++i)
                file_.write(offset + i * 16, erased, 16);
        } else {
            // truncate emulated flash file
            file_.resize(size);
        }
    }
}


// Buffer

Flash_File::Buffer::Buffer(int capacity, Flash_File &device)
    : coco::Buffer(&address_, 4, new uint8_t[capacity], capacity, Buffer::State::READY), device_(device)
{
}

Flash_File::Buffer::~Buffer() {
    delete [] data_;
}

bool Flash_File::Buffer::start() {
    if ((op_ & (Op::READ_WRITE | Op::ERASE)) == 0) {
        setSuccess();
        return false;
    }

    // get address from header and check alignment
    auto address = address_;
    assert((address & (device_.blockSize_ - 1)) == 0);

    // get data and size
    auto data = data_;
    auto size = size_;

    auto &file = device_.file_;
    if ((op_ & Op::ERASE) == 0) {
        // read or write: check range
        assert(address + size <= device_.size_);
        if ((op_ & Op::WRITE) == 0) {
            // read
            file.read(address, data, size);
        } else {
            // write
            file.write(address, data, size);
        }
    } else {
        // erase page

        // align address to page
        int pageSize = device_.pageSize_;
        uint32_t a = address & (pageSize - 1);
        assert(a < device_.size_);

        // erase
        for (int i = 0; i < pageSize; i += 16) {
            file.write(a + i, erased, 16);
        }
    }

    // state stays READY, therefore return false
    return false;
}

bool Flash_File::Buffer::cancel() {
    return true;
}

} // namespace coco
