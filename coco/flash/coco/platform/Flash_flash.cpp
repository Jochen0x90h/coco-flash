#include "Flash_flash.hpp"
#include <coco/debug.hpp>


namespace coco {

bool Flash_flash::BufferBase::start(Op op) {
    // check if READ, WRITE or ERASE flag is set
    assert((op & (Op::READ_WRITE | Op::ERASE)) != 0);

    // get header
    int headerSize = this->p.headerSize;
    if (headerSize != 4) {
        // unsupported header size
        assert(false);
        return false;
    }
    auto header = this->p.data;

    // get address and check alignment
    uint32_t address = *(int32_t *)header;
    assert((address & (BLOCK_SIZE - 1)) == 0);

    auto data = header + headerSize;
    int size = this->p.size - headerSize;

    if ((op & (Op::WRITE | Op::ERASE)) == 0) {
        // read
        auto src = (const flash::Block *)address;
        auto end = src + uint32_t(size + sizeof(flash::Block) - 1) / sizeof(flash::Block);
        auto dst = (flash::Block *)data;
        while (src < end) {
            // read block
            *dst = *src;
            ++src;
            ++dst;
        }
    } else if ((op & Op::ERASE) == 0) {
        // write
        flash::write(address, data, size);
    } else {
        // erase page
        flash::erase(address, this->device.eraseSize);
    }

    setReady(size);
    return true;
}

bool Flash_flash::BufferBase::cancel() {
    return true;
}

} // namespace coco
