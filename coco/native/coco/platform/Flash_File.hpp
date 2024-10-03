#pragma once

#include <coco/Buffer.hpp>
#include <coco/String.hpp>
#include <coco/platform/File.hpp>


namespace coco {

/**
 * Blocking implementation of flash interface based on files, mainly for testing purposes.
 */
class Flash_File {
public:
    /**
     * Constructor
     * @param name file name
     * @param size size of emulated flash
     * @param pageSize size of one page that can be erased at once
     * @param blockSize size of one block that can be written at once and is the read alignment
     */
    Flash_File(String name, int size, int pageSize, int blockSize);

    class Buffer : public coco::Buffer {
    public:
        /**
         * Constructor
         */
        Buffer(int size, Flash_File &file);
        ~Buffer() override;

        bool start(Op op) override;
        bool cancel() override;

    protected:
        Flash_File &file;
    };

protected:
    File file;

    // size of emulated flash
    uint32_t size;

    // size of flash page (can be erased at once)
    int pageSize;

    // size of block (can be written at once)
    int blockSize;
};

} // namespace coco
