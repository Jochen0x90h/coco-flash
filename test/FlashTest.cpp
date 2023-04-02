#include <coco/loop.hpp>
#include <coco/debug.hpp>
#include <coco/Array.hpp>
#include <FlashTest.hpp>


using namespace coco;

const uint8_t writeData[] = {0x12, 0x34, 0x56, 0x78};
uint8_t rd[4];

Coroutine test(Loop &loop, Buffer &buffer) {

	int32_t address = STORAGE_ADDRESS;
	buffer.setHeader(address);
	co_await buffer.read(4);
	if (buffer.array<uint8_t>() == writeData) {
		// blue indicates that the data is there from the last run
		debug::setBlue();

		// erase
		co_await buffer.erase();

		// also switch on red and green leds in case erase did not work
		co_await buffer.read(4);
		if (buffer.array<uint8_t>() == writeData) {
			debug::setRed();
			debug::setGreen();
		}
	} else {
		// erase
		co_await buffer.erase();

		// write data
		buffer.set(writeData);
		co_await buffer.write();

		// read data and check if equal
		co_await buffer.read(4);
		if (buffer.array<uint8_t>() == writeData) {
			// green indicates that write and read was successful
			debug::setGreen();
		} else {
			// red indicates that write or read failed
			debug::setRed();
		}
	}

#ifdef NATIVE
	// ensure that the event loop has started
	co_await loop.yield();

	// exit event loop
	loop.exit();

	// bug: exit gets detected only if yield is pending
	co_await loop.yield();
#endif
}

int main() {
	debug::init(); // for debug led's
	Drivers drivers;

	test(drivers.loop, drivers.buffer);

	drivers.loop.run();
}
