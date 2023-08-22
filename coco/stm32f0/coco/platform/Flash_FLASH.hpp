#pragma once

#include <coco/align.hpp>
#include <coco/BufferImpl.hpp>


namespace coco {

/**
	Blocking implementation of flash interface for stm32f0.

	Reference manual:
		https://www.st.com/resource/en/reference_manual/dm00031936-stm32f0x1stm32f0x2stm32f0x8-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
			Flash: Section 3

	Resources:
		FLASH: Flash controller
*/
namespace Flash_FLASH {
#ifdef STM32F0
	// size of a page that has to be erased at once
	constexpr int PAGE_SIZE = 1024;

	// size of a block that has to be written at once and is the read alignment
	constexpr int BLOCK_SIZE = 2;

	using Block = uint16_t;
#endif
#ifdef STM32G4
	// size of a page that has to be erased at once
	constexpr int PAGE_SIZE = 2048;

	// size of a block that has to be written at once and is the read alignment
	constexpr int BLOCK_SIZE = 8;

	struct Block {
		uint32_t data[2];
	};
#endif


	class BufferBase : public BufferImpl {
	public:
		/**
			Constructor
		*/
		BufferBase(uint8_t *data, int capacity) : BufferImpl(data, capacity, Buffer::State::READY) {}

		bool setHeader(const uint8_t *data, int size) override;
		bool startInternal(int size, Op op) override;
		void cancel() override;

	protected:
		uint32_t address = 1; // 1 is invalid
	};

	/**
		Buffer for transferring data to/from internal flash
		Capacity gets aligned to BLOCK_SIZE
		@tparam N size of buffer
	*/
	template <int N>
	class Buffer : public BufferBase {
	public:
		Buffer() : BufferBase(data, align(N, BLOCK_SIZE)) {}

	protected:
		alignas(4) uint8_t data[align(N, BLOCK_SIZE)];
	};

}

} // namespace coco
