#pragma once

#include <coco/align.hpp>
#include <coco/Buffer.hpp>


namespace coco {

/**
 * Blocking implementation of flash interface for nrf52.
 *
 * Reference manual:
 *   https://infocenter.nordicsemi.com/topic/ps_nrf52840/memory.html?cp=5_0_0_3_1_1#flash
 *   https://infocenter.nordicsemi.com/index.jsp?topic=%2Fps_nrf52840%2Fnvmc.html&cp=5_0_0_3_2
 *
 * Resources:
 *   NRF_NVMC
 */
namespace Flash_NVMC {

    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 4096;

    // size of a block that has to be written at once and is the read alignment
    constexpr int BLOCK_SIZE = 4;

    using Block = uint32_t;


    class BufferBase : public coco::Buffer {
    public:
        /**
         * Constructor
         */
        BufferBase(uint8_t *data, int capacity) : coco::Buffer(data, 4, capacity, Buffer::State::READY) {}

        bool start(Op op) override;
        bool cancel() override;

    protected:
    };

    /**
     * Buffer for transferring data to/from internal flash.
     * Capacity gets aligned to BLOCK_SIZE
     * @tparam C capacity of buffer
     */
    template <int C>
    class Buffer : public BufferBase {
    public:
        Buffer() : BufferBase(data, align(C, BLOCK_SIZE)) {}

    protected:
        // align size because read/write operates on whole blocks
        alignas(4) uint8_t data[4 + align(C, BLOCK_SIZE)];
    };
}

} // namespace coco
