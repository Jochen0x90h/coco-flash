#include <coco/loop.hpp>
#include <coco/debug.hpp>
#include <coco/Array.hpp>
#include <FlashTest.hpp>


using namespace coco;

const uint8_t writeData[] = {0x12, 0x34, 0x56, 0x78};
uint8_t rd[4];


void test(Flash &flash) {
	Array<uint8_t, 4> readData(rd);

	flash.readBlocking(0, readData);
	if (readData == writeData) {
		// blue indicates that the data is there from the last run
		debug::setBlue();

		// erase
		flash.eraseSectorBlocking(0);

		// also switch on red and green leds in case erase did not work
		flash.readBlocking(0, readData);
		if (readData == writeData) {
			debug::setRed();
			debug::setGreen();
		}

		return;
	}

	// erase
	flash.eraseSectorBlocking(0);

	// write data
	flash.writeBlocking(0, writeData);

	// read data and check if equal
	flash.readBlocking(0, readData);
	if (readData == writeData) {
		// green indicates that write and read was successful
		debug::setGreen();
	} else {
		// red indicates that write or read failed
		debug::setRed();
	}
}

int main() {
	debug::init(); // for debug led's
	Drivers drivers;

	test(drivers.flash);

	drivers.loop.run();
}
