#pragma once

#include <coco/align.hpp>
#include <coco/Buffer.hpp>


namespace coco {

/// @brief Blocking implementation of flash interface for stm32f0.
///
/// Reference manual:
///   F0: https://www.st.com/resource/en/reference_manual/dm00031936-stm32f0x1stm32f0x2stm32f0x8-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
///     Section 3
///   G4: https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
//      Sections 3 - 5
///   U3: https://www.st.com/resource/en/reference_manual/rm0487-stm32u3-series-armbased-32bit-mcus-stmicroelectronics.pdf
///     Section 7
/// Resources:
///   FLASH: Flash controller
namespace Flash_FLASH {
#ifdef STM32F0
    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 1024;

    // block of data that has to be written at once and is the read alignment
    using Block = uint16_t;
#endif
#ifdef STM32C0
    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 2048;

    // block of data that has to be written at once and is the read alignment
    struct Block {
        uint32_t data[2];
    };
#endif
#ifdef STM32F3
    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 2048;

    // block of data that has to be written at once
    using Block = uint16_t;
#endif
/*#ifdef STM32F4
    // STM32F4 has sectors of 16k (16384), 64k (65536) and and 128k (131072) bytes

    // block of data that has to be written at once (SMT32F4 supports multiple block sizes, use 32 bit for now)
    using Block = uint32_t;
#endif*/
#ifdef STM32G4
    // size of a page that has to be erased at once (dual bank mode, DBANK in OPTR = 1)
    constexpr int PAGE_SIZE = 2048;

    // block of data that has to be written at once
    struct Block {
        uint32_t data[2];
    };
#endif
#ifdef STM32U3
    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 4096;

    // block of data that has to be written at once
    struct Block {
        uint32_t data[2];
    };
#endif
#ifdef STM32U5
    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 8192;

    // block of data that has to be written at once
    struct Block {
        uint32_t data[4];
    };
#endif
#ifdef STM32H5
    // size of a page that has to be erased at once
    constexpr int PAGE_SIZE = 8192;

    // block of data that has to be written at once
    struct Block {
        uint32_t data[4];
    };
#endif

    // size of a block that has to be written at once and is the read alignment
    constexpr int BLOCK_SIZE = sizeof(Block);


    class BufferBase : public coco::Buffer {
    public:
        /// @brief
        /// Constructor
        BufferBase(uint8_t *data, int capacity) : coco::Buffer(data, 4, capacity, Buffer::State::READY) {}

        bool start(Op op) override;
        bool cancel() override;

    protected:
    };

    /// @brief Buffer for transferring data to/from internal flash
    /// Capacity gets aligned to BLOCK_SIZE
    /// @tparam C capacity of buffer
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
