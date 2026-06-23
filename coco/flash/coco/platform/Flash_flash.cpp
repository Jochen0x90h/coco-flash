#include "Flash_flash.hpp"
#include <coco/debug.hpp>
#include <coco/platform/cache.hpp>


namespace coco {

bool Flash_flash::BufferBase::start() {
    if ((op_ & (Op::READ_WRITE | Op::ERASE)) == 0) {
        setSuccess();
        return false;
    }

    // get address and check alignment
    auto address = address_;
    assert((address & (BLOCK_SIZE - 1)) == 0);

    // get data and size
    auto data = (flash::Word *)data_;
    auto size = size_;

    if ((op_ & (Op::WRITE | Op::ERASE)) == 0) {
        // read
        auto src = (const flash::Word *)address;
        auto end = src + uint32_t(size + sizeof(flash::Word) - 1) / sizeof(flash::Word);
        auto dst = data;
        while (src < end) {
            // read block
            *dst = *src;
            ++src;
            ++dst;
        }
    } else {
        if ((op_ & Op::ERASE) == 0) {
            // write
            flash::write(address, data, size);
        } else {
            // erase page
            flash::erase(address, device_.eraseSize_);
        }

        // flush cache so that subsequent read accesses see the new data
        cache::flush();
    }

    // state stays READY, therefore return false
    return false;
}

bool Flash_flash::BufferBase::cancel() {
    return true;
}

} // namespace coco
