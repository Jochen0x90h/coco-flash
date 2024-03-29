#include <coco/loop.hpp>
#include <coco/debug.hpp>
#include <coco/Array.hpp>
#include <FlashTest.hpp>


using namespace coco;

const uint32_t writeData[] = {12345678, 0x9abcdef0};
const uint32_t erasedData[] = {0xffffffff, 0xffffffff};
uint32_t rd[2];

Coroutine test(Loop &loop, Buffer &buffer) {
	buffer.setHeader<uint32_t>(FLASH_TEST_ADDRESS);
	co_await buffer.read(8);
	if (buffer.transferredArray<uint32_t>() == writeData) {
		// blue indicates that the data is there from the last run
		debug::set(debug::BLUE);

		// erase
		co_await buffer.erase();

		// check if erase worked
		co_await buffer.read(8);
		if (buffer.transferredArray<uint32_t>() != erasedData) {
			debug::set(debug::BLACK);
		}
	} else {
		// erase
		co_await buffer.erase();

		// check if erase succeeded
		co_await buffer.read(8);
		if (buffer.transferredArray<uint32_t>() == erasedData) {

			// write data
			co_await buffer.writeArray(writeData);

			// read data and check if equal
			co_await buffer.read(8);
			if (buffer.transferredArray<uint32_t>() == writeData) {
				// green indicates that write and read was successful
				debug::set(debug::GREEN);
			} else {
				// error: write failed
				debug::set(debug::RED);
			}
		} else {
			// error: erase failed
			debug::set(debug::MAGENTA);
		}
	}

#ifdef NATIVE
	// ensure that the event loop has started
	co_await loop.yield();

	// exit event loop
	loop.exit();
#endif
}

int main() {
	Drivers drivers;

	test(drivers.loop, drivers.buffer);

	drivers.loop.run();
}
