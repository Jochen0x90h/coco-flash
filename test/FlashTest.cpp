#include <coco/loop.hpp>
#include <coco/debug.hpp>
#include <coco/Array.hpp>
#include <coco/board/FlashTest.hpp>


using namespace coco;

const uint8_t writeData[] = {0x12, 0x34, 0x56, 0x78};
uint8_t rd[4];


void test() {
	board::FlashTest drivers;
	Array<uint8_t, 4> readData(rd);

	drivers.flash.readBlocking(0, readData);
	if (readData == writeData) {
		// blue indicates that the data is there from the last run
		debug::setBlue();

		// erase
		drivers.flash.eraseSectorBlocking(0);

		// also switch on red and green leds in case erase did not work
		drivers.flash.readBlocking(0, readData);
		if (readData == writeData) {
			debug::setRed();
			debug::setGreen();
		}

		return;
	}

	// erase
	drivers.flash.eraseSectorBlocking(0);

	// write data
	drivers.flash.writeBlocking(0, writeData);

	// read data and check if equal
	drivers.flash.readBlocking(0, readData);
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

	test();

	loop::run();
}
