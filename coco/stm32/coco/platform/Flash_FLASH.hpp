#pragma once

#include <coco/align.hpp>
#include <coco/Buffer.hpp>


namespace coco {

/**
 * Blocking implementation of flash interface for stm32f0.
 *
 * Reference manual:
 *   https://www.st.com/resource/en/reference_manual/dm00031936-stm32f0x1stm32f0x2stm32f0x8-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
 *     Flash: Section 3
 *
 * Resources:
 *   FLASH: Flash controller
 */
namespace Flash_FLASH {
#ifdef STM32F0
    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 1024;

    // block of data that has to be written at once and is the read alignment
    using Block = uint16_t;
#endif
#ifdef STM32F3
    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 2048;

    // block of data that has to be written at once and is the read alignment
    using Block = uint16_t;
#endif
/*#ifdef STM32F4
    // STM32F4 has sectors of 16k (16384), 64k (65536) and and 128k (131072) bytes

    // block of data that has to be written at once and is the read alignment (SMT32F4 supports multiple block sizes, let's use 32 bit)
    using Block = uint32_t;
#endif*/
#ifdef STM32C0
    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 2048;

    // block of data that has to be written at once and is the read alignment
    struct Block {
        uint32_t data[2];
    };
#endif
#ifdef STM32G4
    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 2048;

    // block of data that has to be written at once and is the read alignment
    struct Block {
        uint32_t data[2];
    };
#endif

    // size of a block that has to be written at once and is the read alignment
    constexpr int BLOCK_SIZE = sizeof(Block);


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
     * Buffer for transferring data to/from internal flash
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
