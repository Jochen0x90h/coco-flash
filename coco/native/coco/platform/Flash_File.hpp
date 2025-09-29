#pragma once

#include <coco/Buffer.hpp>
#include <coco/String.hpp>
#include <coco/platform/File.hpp>


namespace coco {

/// @brief Blocking implementation of flash buffer based on files, mainly for testing purposes.
///
class Flash_File {
public:
    /// @brief Constructor
    /// @param name File name
    /// @param size Size of emulated flash
    /// @param pageSize Size of one page that can be erased at once
    /// @param blockSize Size of one block that can be written at once and is the read alignment
    Flash_File(String name, int size, int pageSize, int blockSize);

    class Buffer : public coco::Buffer {
    public:
        /// @brief Constructor.
        /// The header capacity is always 4
        /// @param capacity Buffer capacity
        Buffer(int capacity, Flash_File &device);
        ~Buffer() override;

        bool start(Op op) override;
        bool cancel() override;

    protected:
        Flash_File &device;
        uint32_t address;
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
