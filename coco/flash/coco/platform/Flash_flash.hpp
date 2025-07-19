#pragma once

#include <coco/align.hpp>
#include <coco/Buffer.hpp>
#include <coco/platform/flash.hpp>


namespace coco {

/// @brief Blocking implementation of flash buffer based on coco::flash HAL.
///
class Flash_flash {
public:
    /// @brief Constructor
    /// @param eraseSize Size to erase, partial flash pages are erased entirely
    Flash_flash(int eraseSize = 1) : eraseSize(eraseSize) {}

    class BufferBase : public coco::Buffer {
    public:
        /// @brief
        /// Constructor
        BufferBase(uint8_t *data, int capacity, Flash_flash &device)
            : coco::Buffer(data, 4, capacity, Buffer::State::READY), device(device) {}

        bool start(Op op) override;
        bool cancel() override;

    protected:
        Flash_flash &device;
    };

    /// @brief Buffer for transferring data to/from internal flash
    /// Capacity gets aligned to BLOCK_SIZE
    /// @tparam C capacity of buffer
    template <int C>
    class Buffer : public BufferBase {
    public:
        Buffer(Flash_flash &device) : BufferBase(data, align(C, flash::BLOCK_SIZE), device) {}

    protected:
        // align size because read/write operates on whole blocks
        alignas(4) uint8_t data[4 + align(C, flash::BLOCK_SIZE)];
    };

protected:
    // size of flash page (can be erased at once)
    int eraseSize;
};

} // namespace coco
