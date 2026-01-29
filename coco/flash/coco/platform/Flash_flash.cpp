#include "Flash_flash.hpp"
#include <coco/debug.hpp>
#include <coco/platform/cache.hpp>


namespace coco {

bool Flash_flash::BufferBase::start(Op op) {
    // check if READ, WRITE or ERASE flag is set
    assert((op & (Op::READ_WRITE | Op::ERASE)) != 0);

    // get address and check alignment
    auto address = address_;
    assert((address & (BLOCK_SIZE - 1)) == 0);

    // get data and size
    auto data = (flash::Block *)data_;
    auto size = size_;

    if ((op & (Op::WRITE | Op::ERASE)) == 0) {
        // read
        auto src = (const flash::Block *)address;
        auto end = src + uint32_t(size + sizeof(flash::Block) - 1) / sizeof(flash::Block);
        auto dst = data;
        while (src < end) {
            // read block
            *dst = *src;
            ++src;
            ++dst;
        }
    } else {
        if ((op & Op::ERASE) == 0) {
            // write
            flash::write(address, data, size);
        } else {
            // erase page
            flash::erase(address, device_.eraseSize_);
        }

        // flush cache so that subsequent read accesses see the new data
        cache::flush();
    }

    setReady(size);
    return true;
}

bool Flash_flash::BufferBase::cancel() {
    return true;
}

} // namespace coco
