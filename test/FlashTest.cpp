#include <coco/loop.hpp>
#include <coco/convert.hpp>
#include <coco/debug.hpp>
#include <coco/Array.hpp>
#include <FlashTest.hpp>


// Flash test
// Check debug output in a terminal (115200 8N1)
// Green indicates that data was successfully written
// Blue indicates that data was present from last run and was erased (off when no blue LED is present)
// Power cycling should cause in toggle between green and blue (off)

using namespace coco;

const uint32_t writeData[] = {0x12345678, 0x9abcdef0};
const uint32_t erasedData[] = {0xffffffff, 0xffffffff};
uint32_t rd[2];

Coroutine test(Loop &loop, Buffer &buffer) {
    buffer.header<uint32_t>() = FLASH_TEST_ADDRESS;
    co_await buffer.read(8);
    if (buffer.array<uint32_t>() == writeData) {
        debug::out << "Data found from last run\n";

        // blue indicates that the data is there from the last run
        debug::set(debug::BLUE);

        // erase
        debug::out << "Erase ";
        co_await buffer.erase();

        // check if erase worked
        co_await buffer.read(8);
        if (buffer.array<uint32_t>() == erasedData) {
            debug::out << "success!\n";
        } else {
            debug::out << "error!\n";
            debug::set(debug::BLACK);
        }
    } else {
        // erase
        debug::out << "Erase ";
        co_await buffer.erase();

        // check if erase succeeded
        co_await buffer.read(8);
        if (buffer.array<uint32_t>() == erasedData) {
            debug::out << "success!\n";

            // write data
            debug::out << "Write ";
            co_await buffer.writeArray(writeData);

            // read data and check if equal
            co_await buffer.read(8);
            if (buffer.array<uint32_t>() == writeData) {
                debug::out << "success!\n";

                // green indicates that write and read was successful
                debug::set(debug::GREEN);
            } else {
                // error: write failed
                debug::out << "error!\n";
                debug::set(debug::RED);
            }
        } else {
            // error: erase failed
            debug::out << "error!\n";
            debug::set(debug::MAGENTA);
        }
    }

#ifdef NATIVE
    // exit event loop
    loop.exit();
#endif
}

int main() {
    debug::out << "FlashTest\n";
    debug::out << "FLASH_ADDRESS " << hex(FLASH_ADDRESS) << '\n';
    debug::out << "FLASH_SIZE " << hex(FLASH_SIZE) << '\n';

    test(drivers.loop, drivers.buffer);

    drivers.loop.run();
}
