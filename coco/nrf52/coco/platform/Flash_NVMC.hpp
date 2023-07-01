#pragma once

#include <coco/BufferImpl.hpp>


namespace coco {

/**
	Blocking implementation of flash interface for nrf52.

	Reference manual:
		https://infocenter.nordicsemi.com/topic/ps_nrf52840/memory.html?cp=5_0_0_3_1_1#flash
		https://infocenter.nordicsemi.com/index.jsp?topic=%2Fps_nrf52840%2Fnvmc.html&cp=5_0_0_3_2

	Resources:
		NRF_NVMC
*/
namespace Flash_NVMC {

	// size of a page that has to be erased at once
	constexpr int PAGE_SIZE = 4096;

	// size of a block that has to be written at once and is the read alignment
	constexpr int BLOCK_SIZE = 4;

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
		Buffer for transferring data to/from internal flash.
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
