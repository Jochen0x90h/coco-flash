#pragma once

#include <coco/align.hpp>
#include <coco/Buffer.hpp>
#include <coco/platform/flash.hpp>


namespace coco {

/// @brief Blocking implementation of flash buffer based on coco::flash hardware abstraction layer.
///
class Flash_flash {
public:
    /// @brief Constructor
    /// @param eraseSize Size to erase, partial flash pages are erased entirely
    Flash_flash(int eraseSize = 1) : eraseSize_(eraseSize) {}

    class BufferBase : public coco::Buffer {
    public:
        /// @brief Constructor.
        /// The header capacity is always 4
        /// @param headerAndData Header and data buffer
        /// @param capacity Buffer capacity
        BufferBase(uint8_t *data, int capacity, Flash_flash &device)
            : coco::Buffer(&address_, 4, data, capacity, Buffer::State::READY), device_(device) {}

        bool start() override;
        bool cancel() override;

    protected:
        Flash_flash &device_;
        uint32_t address_;
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
        alignas(4) uint8_t data[align(C, flash::BLOCK_SIZE)];
    };

protected:
    // size of flash page (can be erased at once)
    int eraseSize_;
};

} // namespace coco
